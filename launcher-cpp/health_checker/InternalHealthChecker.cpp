//
// Created by bimba on 11/22/25.
//

#include "InternalHealthChecker.hpp"

#include "../logger/LoggerFactory.hpp"

InternalHealthChecker::InternalHealthChecker(const IModule* module, const Services* services) : IHealthChecker(module),
    services_(services)
{
    logger_ = services->get<LoggerFactory>()->create("InternalHealthChecker-" + module->name());
}

InternalHealthChecker::~InternalHealthChecker()
{
    services_->get<LoggerFactory>()->drop(logger_);
}

HealthStatus InternalHealthChecker::checkStatus() const
{
    if (!module_) throw std::runtime_error("InternalHealthChecker: module is null");
    if (module_->loaded()) return HealthStatus::HEALTHY;
    return HealthStatus::NOT_LOADED;
}
