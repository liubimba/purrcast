//
// Created by bimba on 11/15/25.
//

#include "os_process.hpp"


os_process::os_process(const services* services, std::string id):
    i_process(services, std::move(id))
{
#if defined(__linux__)
    instance_ = std::make_unique<linux_process>(services, id_);
#endif
}

os_process::~os_process() = default;

process_result os_process::execute(const std::string& command, const std::string& arguments)
{
    return instance_->execute(command, arguments);
}

void os_process::add_listener(i_process_listener* listener)
{
    instance_->add_listener(listener);
}

process_result os_process::terminate()
{
    return instance_->terminate();
}

process_state os_process::state()
{
    return instance_->state();
}
