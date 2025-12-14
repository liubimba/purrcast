//
// Created by bimba on 11/22/25.
//

#include "TcpHealthChecker.hpp"


TCPHealthChecker::TCPHealthChecker(const IModule* module, const Services* services, std::initializer_list<int> ports) :
    InternalHealthChecker(module, services),
    services_(services), ports_(ports)
{
    logger_ = services->get<LoggerFactory>()->create("TCPHealthChecker-" + module->name());
}

TCPHealthChecker::~TCPHealthChecker()
{
    services_->get<LoggerFactory>()->drop(logger_);
}

HealthStatus TCPHealthChecker::checkStatus() const
{
    HealthStatus status = InternalHealthChecker::checkStatus();
    if (status == HealthStatus::HEALTHY)
    {
        for (auto it = ports_.begin(); it != ports_.end() && status == HealthStatus::HEALTHY; ++it)
        {
            int port = *it;
            logger_->info("Check port {}", port);
            if (!Port::isBound(port))
            {
                logger_->error("Expected that port {} is bound, but its free", port);
                status = HealthStatus::UNHEALTHY;
            }
        }
    }
    return status;
}
