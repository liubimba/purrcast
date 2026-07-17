//
// Created by bimba on 11/15/25.
//

#include "linux_process.hpp"

#include <csignal>
#include <sys/wait.h>

#include "../logger/logger_factory.hpp"
#include "absl/strings/str_format.h"

namespace
{
    const char* name_of(process_state state)
    {
        switch (state)
        {
        case process_state::NEW: return "NEW";
        case process_state::EXECUTING: return "EXECUTING";
        case process_state::RUNNING: return "RUNNING";
        case process_state::TERMINATING: return "TERMINATING";
        case process_state::TERMINATED: return "TERMINATED";
        case process_state::FAILED: return "FAILED";
        }
        return "UNKNOWN";
    }
}


linux_process::linux_process(const services* services, const std::string& id):
    i_process(services, id),
    timeout_(std::chrono::seconds(1)),
    executor_(std::make_unique<task_executor>(services, id)),
    state_(process_state::NEW)

{
    logger_ = services->get<logger_factory>()->create("LinuxProcess-" + id);
}

linux_process::~linux_process()
{
    logger_->info("Destructor");
    if (state_ != process_state::TERMINATING && state_ != process_state::TERMINATED && state_ != process_state::FAILED)
    {
        logger_->warn("Destructor was called while process is running or pending");
        terminate_internal_();
    }
    if (executor_->joinable())
        executor_->join();
    if (monitor_thread_.joinable())
        monitor_thread_.join();
    services_->get<logger_factory>()->drop(logger_);
}

process_result linux_process::execute(const std::string& cmd, const std::string& args)
{
    if (cmd.empty())
        throw std::runtime_error("LinuxProcess::execute() called with empty command");
    process_state expected = process_state::NEW;
    if (!state_.compare_exchange_strong(expected, process_state::EXECUTING))
        throw std::runtime_error("LinuxProcess::execute() called with state different to ProcessState::NEW");
    logger_->info("Post task to execute: {} {}", cmd, args);
    std::future<bool> future = executor_->post<bool>([=] { return execute_internal_(cmd, args); });
    if (future.wait_for(timeout_) == std::future_status::ready && future.get())
    {
        monitor_thread_ = std::thread(&linux_process::monitor_, this);
        state_ = process_state::RUNNING;
        return process_result::ok(absl::StrFormat("Executed: %s %s. PID: %d", cmd, args, pid_));
    }
    state_ = process_state::FAILED;
    return process_result::failed(absl::StrFormat("Failed or timed out to execute command: %s %S", cmd, args));
}

process_result linux_process::terminate()
{
    process_state observed = state_.load();
    for (;;)
    {
        if (observed != process_state::RUNNING && observed != process_state::EXECUTING)
            throw std::runtime_error(
                absl::StrFormat("LinuxProcess::terminate() called in state %s", name_of(observed)));
        if (state_.compare_exchange_weak(observed, process_state::TERMINATING))
            break;
    }
    logger_->info("Post task to terminate");
    std::future<bool> future = executor_->post<bool>([this] { return terminate_internal_(); });
    if (future.wait_for(timeout_) == std::future_status::ready && future.get())
    {
        state_ = process_state::TERMINATED;
        return process_result::ok();
    }
    state_ = process_state::FAILED;
    return process_result::failed(absl::StrFormat("Failed or timed out to terminate"));
}

void linux_process::add_listener(i_process_listener* listener)
{
    listeners_.insert(listener);
}

process_state linux_process::state()
{
    return state_;
}

void linux_process::monitor_()
{
    const pid_t pid = pid_.load();
    logger_->info("[Monitor]: wait for pid: {}", pid);
    if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) != 0)
            logger_->error("[Monitor]: forked process exit unsuccessfully. Status: ", status);
        else
            logger_->info("[Monitor]: forked process exited");
        state_ = process_state::TERMINATED;
        pid_.store(-1);
        std::for_each(listeners_.begin(), listeners_.end(),
                      [](i_process_listener* listener) { listener->onTerminate(); });
    }
    logger_->info("[Monitor]: completed monitoring");
}

bool linux_process::execute_internal_(const std::string& cmd, const std::string& args)
{
    int fd[2];
    if (pipe(fd) == -1)
    {
        logger_->error("Failed to create pipe");
        return false;
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        logger_->info("Executing on forked process: {} {}", cmd, args);
        const char* msg = "Hello, from fork!";
        close(fd[0]);
        write(fd[1], msg, std::strlen(msg) + 1);
        setpgid(0, 0);
        execl("/bin/sh", "sh", "-c", (cmd + " " + args).c_str(), nullptr);
        close(fd[1]);
        std::exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        close(fd[1]);
        char buffer[32];
        if (ssize_t recv = read(fd[0], buffer, sizeof(buffer)) < 0)
            logger_->error(
                "Failed to read from pipe. Possibly state accept RUNNING before forked process really execute command");
        else
            logger_->info("Received from forked process: {}", buffer);
        close(fd[0]);
        pid_ = pid;
    }
    else
    {
        logger_->error("Failed to fork");
        return false;
    }
    return true;
}

bool linux_process::terminate_internal_()
{
    logger_->info("Terminating");
    const pid_t pid = pid_.exchange(-1);
    if (pid <= 0)
    {
        logger_->info("Already reaped; nothing to signal");
        return true;
    }
    if (kill(-pid, SIGTERM) != 0)
    {
        logger_->error("kill() {} failed: {}", pid, strerror(errno));
        return false;
    }
    if (monitor_thread_.joinable())
    {
        logger_->info("Monitor thread join");
        monitor_thread_.join();
        logger_->info("Monitor thread joined");
    }
    logger_->info("Notify listeners about termination");
    std::for_each(listeners_.begin(), listeners_.end(),
                  [](i_process_listener* listener) { listener->onTerminate(); });
    return true;
}

