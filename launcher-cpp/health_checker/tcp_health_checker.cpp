//
// Created by bimba on 11/22/25.
//

#include "tcp_health_checker.hpp"


tcp_health_checker::tcp_health_checker(const i_module* module, const services* services, std::initializer_list<int> ports) :
    internal_health_checker(module, services),
    services_(services), ports_(ports)
{
    logger_ = services->get<logger_factory>()->create("TCPHealthChecker-" + module->name());
}

tcp_health_checker::~tcp_health_checker()
{
    services_->get<logger_factory>()->drop(logger_);
}

health_status tcp_health_checker::check() const
{
    health_status status = internal_health_checker::check();
    if (status == health_status::healthy)
    {
        for (auto it = ports_.begin(); it != ports_.end() && status == health_status::healthy; ++it)
        {
            int port = *it;
            logger_->debug("Check port {}", port);
            if (!os_port::is_bound(port))
            {
                logger_->error("Expected that port {} is bound, but its free", port);
                status = health_status::unhealthy;
            }
        }
    }
    return status;
}
