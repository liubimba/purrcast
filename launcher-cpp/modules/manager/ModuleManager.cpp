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

void ModuleManager::add(const module_session& session)
{
    const module_description& description = module_cast(session.params);
    sessions_.insert({description.module_name, session});
    logger_->info("Add module session: {}", description.module_name);
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

std::vector<ModuleStatus> ModuleManager::report()
{
    std::vector<ModuleStatus> result;
    for (const auto session : sessions_)
    {
        result.emplace_back(session.second.module->get_last_status());
    }
    return result;
}

void ModuleManager::run_(const settings::s_manager params)
{
    logger_->info("Run");
    auto last_look_up = boost::chrono::system_clock::now();
    auto interval_look_up = params.interval.unhealthy;
    while (running_.load())
    {
        {
            boost::unique_lock lock(mutex_);
            condition_.wait_for(lock, interval_look_up, [this]
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
        if (now - last_look_up > interval_look_up)
        {
            interval_look_up = look_up_() ? params.interval.healthy : params.interval.unhealthy;
            last_look_up = now;
        }
    }
    for (const auto& session : sessions_)
    {
        auto module = session.second.module;
        if (module->loaded())
        {
            try
            {
                module->unload();
            }
            catch (std::exception& e)
            {
            }
        }
    }
}

bool ModuleManager::look_up_()
{
    logger_->debug("Look up");
    std::unordered_set<std::string> unhealthy_modules;
    for (auto& [moduleName, session] : sessions_)
    {
        const module_description& description = module_cast(session.params);
        if (session.checker->check() == health_status::healthy)
        {
            if (!ready_by_dependency(session))
            {
                logger_->info("Module {} is loaded, but it`s dependent module are not healthy. Unload module: {}",
                              description.module_name, description.module_name);
                session.module->unload();
            }
            else
            {
                if (is_running_with_same_params_(session)) continue;
                if (try_reload_(session))
                {
                    session.params = session.module->get_params();
                    continue;
                }
            }
        }
        if (session.checker->check() == health_status::not_loaded)
        {
            if (!try_load_(session))
            {
                unhealthy_modules.insert(moduleName);
                continue;
            }
        }
        if (session.checker->check() == health_status::unhealthy)
        {
            logger_->error("Module is unhealthy: {}", moduleName);
            unhealthy_modules.insert(moduleName);
            continue;
        }
        extra_(session);
    }
    if (unhealthy_modules.empty())
        logger_->info("All modules are loaded and works correctly. Next look up in 5 seconds");
    else
        logger_->error("Unhealthy modules: [{}]. Next look up in 500 milliseconds",
                       absl::StrJoin(unhealthy_modules, ", "));
    return unhealthy_modules.empty();
}

std::vector<std::string> ModuleManager::get_unhealthy_dependent_modules_by_order(const module_session& session)
{
    std::vector<std::string> unhealthy_dependent_modules;
    for (const auto& dependent_session : sessions_)
    {
        if (dependent_session.second == session) continue;
        const module_description& session_description = module_cast(session.params);
        const module_description& dependent_description = module_cast(dependent_session.second.params);

        if (session_description.order_of_loading < dependent_description.order_of_loading &&
            dependent_session.second.checker->check() != health_status::healthy)
        {
            unhealthy_dependent_modules.emplace_back(dependent_session.first);
        }
    }
    return unhealthy_dependent_modules;
}

bool ModuleManager::ready_by_dependency(const module_session& session)
{
    const module_description& description = module_cast(session.params);
    return std::all_of(description.depends_on.begin(), description.depends_on.end(),
                       [this](const std::string& dependent_module_name)
                       {
                           if (sessions_.count(dependent_module_name) == 0)
                               throw std::runtime_error("Dependent module not found");
                           return sessions_.at(dependent_module_name).checker->check() == health_status::healthy;
                       });
}

bool ModuleManager::ready_by_order(const module_session& session)
{
    return get_unhealthy_dependent_modules_by_order(session).empty();
}

bool ModuleManager::try_load_(const module_session& session)
{
    std::unordered_set<std::string> unhealthy_dependent_modules;
    const module_description& description = module_cast(session.params);
    for (const std::string& dependentModuleName : description.depends_on)
    {
        if (!sessions_.count(dependentModuleName))
            throw std::runtime_error("Dependence on unknown module");
        if (sessions_.at(dependentModuleName).checker->check() != health_status::healthy)
            unhealthy_dependent_modules.insert(dependentModuleName);
    }
    if (!unhealthy_dependent_modules.empty())
    {
        logger_->error("Module {} is not ready to load, since dependent module(s) unhealthy: [{}]",
                       description.module_name, absl::StrJoin(unhealthy_dependent_modules, ", "));
        return false;
    }

    std::vector<std::string> unhealthy_dependent_modules_by_order = get_unhealthy_dependent_modules_by_order(session);
    if (!unhealthy_dependent_modules.empty())
    {
        logger_->error("Module {} is not ready to load, since dependent module(s) by order unhealthy: [{}]",
                       description.module_name, absl::StrJoin(unhealthy_dependent_modules_by_order, ", "));
        return false;
    }

    if (!session.module->load(session.params))
    {
        logger_->error("Failed to load module: {}", description.module_name);
        return false;
    }

    return true;
}

bool ModuleManager::try_reload_(const module_session& session)
{
    return session.module->reload(session.params);
}

void ModuleManager::extra_(const module_session& session)
{
    const module_description& description = module_cast(session.params);

    if (description.module_name == "loopback")
    {
        auto loopback_module = dynamic_cast<IAudioLoopbackModule*>(session.module);
        std::string sink_index = loopback_module->get_origin_alsa_device();
        std::string monitor_description = loopback_module->get_loopback_monitor_description();

        audio_device origin_device = loopback_module->get_origin_device();
        audio_device loopback_monitor_device = loopback_module->get_loopback_monitor_device();

        if (sessions_.count("router") != 0)
        {
            std::get<settings::s_module::s_router>(sessions_.at("router").params).source.name =
                loopback_monitor_device.description;
        }
        if (sessions_.count("snapclient") != 0)
        {
            std::get<settings::s_module::s_snapclient>(sessions_.at("snapclient").params).soundcard = origin_device.
                name;
        }
    }
    if (description.module_name == "test_environment_loopback")
    {
        auto loopback_module = dynamic_cast<IAudioLoopbackModule*>(session.module);
        audio_device loopback_device = loopback_module->get_loopback_device();
        audio_device loopback_monitor_device = loopback_module->get_loopback_monitor_device();

        if (sessions_.count("test_environment_snapclient") != 0)
        {
            std::get<settings::s_module::s_snapclient>(sessions_.at("test_environment_snapclient").params).soundcard =
                loopback_device.
                name;
        }
        if (sessions_.count("test_environment") != 0)
        {
            std::get<settings::s_module::s_test_environment>(sessions_.at("test_environment").params).
                loopback_monitor_description = loopback_monitor_device.description;
            std::get<settings::s_module::s_test_environment>(sessions_.at("test_environment").params).
                loopback_name = loopback_device.name;
        }
    }
}

bool ModuleManager::is_running_with_same_params_(const module_session& session)
{
    return session.module->get_params() == session.params;
}
