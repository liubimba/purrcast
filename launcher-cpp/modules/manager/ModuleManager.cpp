//
// Created by bimba on 11/15/25.
//

#include "ModuleManager.hpp"

#include "absl/strings/str_join.h"


ModuleManager::ModuleManager(const Services* services):
    services_(services), running_(false)
{
    logger_ = services_->get<LoggerFactory>()->create("ModuleManager");
}

void ModuleManager::add(const ModuleSession& session)
{
    module_description description = module_cast(session.params);
    sessions_.insert({description.name, session});
    logger_->info("Add module session: {}", description.name);
}

void ModuleManager::startup(const settings::s_manager& params)
{
    logger_->info("Startup");
    running_.store(true);
    thread_ = boost::thread{&ModuleManager::run_, this, params};
}

void ModuleManager::shutdown()
{
    logger_->info("Shutdown");
    running_.store(false);
    condition_.notify_all();
    if (thread_.joinable()) thread_.join();
}

bool ModuleManager::running()
{
    return running_.load();
}

void ModuleManager::run_(struct settings::s_manager params)
{
    logger_->info("Run lookup thread");
    auto lookUpLast = boost::chrono::system_clock::now();
    auto lookUpInterval = params.interval.unhealthy;
    while (running_.load())
    {
        {
            boost::unique_lock lock(mutex_);
            condition_.wait_for(lock, lookUpInterval, [this]
            {
                return !running_.load() || !taskQueue_.empty();
            });
            if (!running_.load()) break;
            if (!taskQueue_.empty())
            {
                lock.unlock();
                taskQueue_.front()();
                taskQueue_.pop();
            }
        }

        auto now = boost::chrono::system_clock::now();
        if (now - lookUpLast > lookUpInterval)
        {
            lookUpInterval = lookUpSessionsState_() ? params.interval.healthy : params.interval.unhealthy;
            lookUpLast = now;
        }
    }
}

bool ModuleManager::lookUpSessionsState_()
{
    logger_->info("Look up modules state");
    std::unordered_set<std::string> unhealthyModules;
    for (auto& [moduleName, session] : sessions_)
    {
        module_description description = module_cast(session.params);
        if (session.checker->checkStatus() == HealthStatus::HEALTHY)
        {
            if (!dependentHealthy(session))
            {
                logger_->info("Module {} is loaded, but it`s dependent module are not healthy. Unload module: {}",
                              description.name, description.name);
                session.module->unload();
            }
            else
            {
                if (runningWithSameParameters_(session)) continue;
                if (tryReload(session))
                {
                    session.params = session.module->getParams();
                    continue;
                }
            }
        }
        if (session.checker->checkStatus() == HealthStatus::NOT_LOADED)
        {
            if (!tryLoad_(session))
            {
                unhealthyModules.insert(moduleName);
                continue;
            }
        }
        if (session.checker->checkStatus() == HealthStatus::UNHEALTHY)
        {
            logger_->error("Module is unhealthy: {}", moduleName);
            unhealthyModules.insert(moduleName);
            continue;
        }
        extraActions_(session);
    }
    if (unhealthyModules.empty())
        logger_->info("All modules are loaded and works correctly. Next look up in 5 seconds");
    else
        logger_->error("Unhealthy modules: [{}]. Next look up in 500 milliseconds",
                       absl::StrJoin(unhealthyModules, ", "));
    return unhealthyModules.empty();
}

bool ModuleManager::dependentHealthy(const ModuleSession& session)
{
    module_description description = module_cast(session.params);
    return std::all_of(description.dependsOn.begin(), description.dependsOn.end(),
                       [this](const std::string& dependentModuleName)
                       {
                           if (sessions_.count(dependentModuleName) == 0)
                               throw std::runtime_error("Dependent module not found");
                           return sessions_.at(dependentModuleName).checker->checkStatus() == HealthStatus::HEALTHY;
                       });
}

bool ModuleManager::tryLoad_(const ModuleSession& session)
{
    std::unordered_set<std::string> unhealthyDependentModules;
    module_description description = module_cast(session.params);
    for (const std::string& dependentModuleName : description.dependsOn)
    {
        if (!sessions_.count(dependentModuleName))
            throw std::runtime_error("Dependence on unknown module");
        if (sessions_.at(dependentModuleName).checker->checkStatus() != HealthStatus::HEALTHY)
            unhealthyDependentModules.insert(dependentModuleName);
    }
    if (!unhealthyDependentModules.empty())
    {
        logger_->error("Module {} is not ready to load, since dependent module(s) unhealthy: [{}]",
                       description.name, absl::StrJoin(unhealthyDependentModules, ", "));
        return false;
    }
    if (!session.module->load(session.params))
    {
        logger_->error("Failed to load module: {}", description.name);
        return false;
    }

    return true;
}

bool ModuleManager::tryReload(const ModuleSession& session)
{
    return session.module->reload(session.params);
}

void ModuleManager::extraActions_(const ModuleSession& session)
{
    module_description description = module_cast(session.params);

    if (description.name == "loopback")
    {
        auto loopbackModule = dynamic_cast<IAudioLoopbackModule*>(session.module);
        uint32_t sinkIndex = loopbackModule->getSinkIndex();
        std::string monitorDescription = loopbackModule->getMonitorDescription();

        std::get<settings::s_module::s_router>(sessions_.at("router").params).source.name = monitorDescription;
        std::get<settings::s_module::s_snapclient>(sessions_.at("snapclient").params).sinkIndex = sinkIndex;
    }
}

bool ModuleManager::runningWithSameParameters_(const ModuleSession& session)
{
    return session.module->getParams() == session.params;
}
