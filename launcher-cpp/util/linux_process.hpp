//
// Created by bimba on 11/15/25.
//

#ifndef LINUXPROCESS_HPP
#define LINUXPROCESS_HPP
#include <future>
#include <queue>
#include <unordered_set>

#include "task_executor.hpp"
#include "i_process.hpp"


class linux_process : public i_process
{
public:
    linux_process(const services* services, const std::string& id);
    ~linux_process() override;

    process_result terminate() override;
    process_result execute(const std::string& cmd, const std::string& args) override;
    void add_listener(i_process_listener* listener) override;
    process_state state() override;

private:
    void monitor_();
    bool execute_internal_(const std::string& cmd, const std::string& args);
    bool terminate_internal_();

    pid_t pid_ = -1;
    std::thread monitor_thread_;
    std::atomic<process_state> state_;
    std::unique_ptr<task_executor> executor_;
    std::chrono::duration<double> timeout_;
    std::shared_ptr<spdlog::logger> logger_;
    std::unordered_set<i_process_listener*> listeners_;
};

#endif //LINUXPROCESS_HPP
