//
// Created by bimba on 11/15/25.
//

#include "Process.hpp"

#include <utility>

#include "LinuxProcess.hpp"
#include "../logger/LoggerFactory.hpp"

Process::Process(const Services* services, std::string id):
    IProcess(services, std::move(id))
{
#if defined(__linux__)
    instance_ = std::make_unique<LinuxProcess>(services, id_);
#endif
}

Process::~Process() = default;

bool Process::execute(const std::string& command, const std::string& arguments)
{
    return instance_->execute(command, arguments);
}

void Process::addListener(IProcessListener* listener)
{
    instance_->addListener(listener);
}

bool Process::terminate()
{
    return instance_->terminate();
}

ProcessState Process::state()
{
    return instance_->state();
}
