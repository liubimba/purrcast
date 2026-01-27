//
// Created by bimba on 11/22/25.
//

#include "internal_health_checker.hpp"

#include "../logger/logger_factory.hpp"

internal_health_checker::internal_health_checker(const i_module* module, const services* services) : i_health_checker(module),
                                                                                                     services_(services)
{
    logger_ = services->get<logger_factory>()->create("InternalHealthChecker-" + module->name());
}

internal_health_checker::~internal_health_checker()
{
    services_->get<logger_factory>()->drop(logger_);
}

health_status internal_health_checker::check() const
{
    if (!module_) throw std::runtime_error("InternalHealthChecker: module is null");
    if (module_->loaded()) return health_status::healthy;
    return health_status::not_loaded;
}
