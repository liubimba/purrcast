//
// Created by bimba on 11/15/25.
//

#include "LinuxProcess.hpp"

#include <csignal>
#include <sys/wait.h>

#include "../logger/LoggerFactory.hpp"


LinuxProcess::LinuxProcess(const Services* services, const std::string& id):
    IProcess(services, id),
    timeout_(std::chrono::seconds(1)),
    executor_(std::make_unique<Executor>(services, id)),
    state_(ProcessState::NEW)

{
    logger_ = services->get<LoggerFactory>()->create("LinuxProcess-" + id);
}

LinuxProcess::~LinuxProcess()
{
    logger_->info("Destructor");
    if (state_ != ProcessState::TERMINATING && state_ != ProcessState::TERMINATED && state_ != ProcessState::FAILED)
    {
        logger_->warn("Destructor was called while process is running or pending");
        terminateInternal_();
    }
    if (executor_->joinable())
        executor_->join();
    if (monitorThread_.joinable())
        monitorThread_.join();
    services_->get<LoggerFactory>()->drop(logger_);
}

bool LinuxProcess::execute(const std::string& cmd, const std::string& args)
{
    if (state_ != ProcessState::NEW)
        throw std::runtime_error("LinuxProcess::execute() called with state different to ProcessState::NEW");
    if (cmd.empty())
        throw std::runtime_error("LinuxProcess::execute() called with empty command");
    state_ = ProcessState::EXECUTING;
    logger_->info("Post task to execute: {} {}", cmd, args);
    std::future<bool> future = executor_->post<bool>([=] { return executeInternal_(cmd, args); });
    if (future.wait_for(timeout_) == std::future_status::ready && future.get())
    {
        logger_->info("Successfully executed: {} {}, pid: {}", cmd, args, pid_);
        monitorThread_ = std::thread(&LinuxProcess::monitor_, this);
        state_ = ProcessState::RUNNING;
        return true;
    }
    logger_->error("Failed or timed out to execute: {} {}", cmd, args);
    state_ = ProcessState::FAILED;
    return false;
}

bool LinuxProcess::terminate()
{
    if (state_ == ProcessState::NEW)
        throw std::runtime_error("LinuxProcess::terminate() called with state NEW");
    if (state_ == ProcessState::FAILED)
        throw std::runtime_error("LinuxProcess::terminate() called with state FAILED");
    if (state_ == ProcessState::TERMINATING)
        throw std::runtime_error("LinuxProcess::terminate() called in state TERMINATING");
    if (state_ == ProcessState::TERMINATED)
        throw std::runtime_error("LinuxProcess::terminate() called in state TERMINATED");
    logger_->info("Post task to terminate");
    state_ = ProcessState::TERMINATING;
    std::future<bool> future = executor_->post<bool>([this] { return terminateInternal_(); });
    if (future.wait_for(timeout_) == std::future_status::ready && future.get())
    {
        state_ = ProcessState::TERMINATED;
        return true;
    }
    logger_->error("Failed or timed out to terminate");
    state_ = ProcessState::FAILED;
    return false;
}

void LinuxProcess::addListener(IProcessListener* listener)
{
    listeners_.insert(listener);
}

ProcessState LinuxProcess::state()
{
    return state_;
}

void LinuxProcess::monitor_()
{
    logger_->info("[Monitor]: wait for pid: {}", pid_);
    if (pid_ > 0)
    {
        int status;
        waitpid(pid_, &status, 0);
        if (WEXITSTATUS(status) != 0)
            logger_->error("[Monitor]: forked process exit unsuccessfully. Status: ", status);
        else
            logger_->info("[Monitor]: forked process exited");
        state_ = ProcessState::TERMINATED;
        pid_ = -1;
        std::for_each(listeners_.begin(), listeners_.end(),
                      [](IProcessListener* listener) { listener->onTerminate(); });
    }
    logger_->info("[Monitor]: completed monitoring");
}

bool LinuxProcess::executeInternal_(const std::string& cmd, const std::string& args)
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

bool LinuxProcess::terminateInternal_()
{
    logger_->info("Terminating");
    int error = kill(-pid_, SIGTERM);
    if (error != 0)
    {
        logger_->error("kill() {} failed: {}", pid_, strerror(errno));
        return false;
    }
    pid_ = -1;
    if (monitorThread_.joinable())
    {
        logger_->info("Monitor thread join");
        monitorThread_.join();
        logger_->info("Monitor thread joined");
    }
    logger_->info("Notify listeners about termination");
    std::for_each(listeners_.begin(), listeners_.end(),
                  [](IProcessListener* listener) { listener->onTerminate(); });
    return true;
}

