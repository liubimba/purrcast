//
// Created by bimba on 11/15/25.
//

#ifndef LINUXPROCESS_HPP
#define LINUXPROCESS_HPP
#include <future>
#include <queue>
#include <unordered_set>

#include "Executor.hpp"
#include "IProcess.hpp"


class LinuxProcess : public IProcess
{
public:
    LinuxProcess(const Services* services, const std::string& id);
    ~LinuxProcess() override;

    bool terminate() override;
    bool execute(const std::string& cmd, const std::string& args) override;
    void addListener(IProcessListener* listener) override;
    ProcessState state() override;

private:
    void monitor_();
    bool executeInternal_(const std::string& cmd, const std::string& args);
    bool terminateInternal_();

    pid_t pid_ = -1;
    std::thread monitorThread_;
    std::atomic<ProcessState> state_;
    std::unique_ptr<Executor> executor_;
    std::chrono::duration<double> timeout_;
    std::shared_ptr<spdlog::logger> logger_;
    std::unordered_set<IProcessListener*> listeners_;
};

#endif //LINUXPROCESS_HPP
