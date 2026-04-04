//
// Created by bimba on 11/15/25.
//

#include "module_manager.hpp"

#include "absl/strings/str_join.h"


module_manager::module_manager(const services* services):
    services_(services), running_(false)
{
    logger_ = services_->get<logger_factory>()->create("ModuleManager");
}

void module_manager::add(const module_session& session)
{
    const module_description& description = module_cast(session.params);
    sessions_.insert({description.module_name, session});
    logger_->info("[+] Registered module: '{}'", description.module_name);
}

void module_manager::startup(const settings::s_manager& params)
{
    logger_->info("Starting up...");
    running_.store(true);
    thread_ = boost::thread{&module_manager::run_, this, params};
}

void module_manager::shutdown()
{
    logger_->info("Shutting down...");
    running_.store(false);
    condition_.notify_all();
    if (thread_.joinable()) thread_.join();
}

bool module_manager::running() const
{
    return running_.load();
}

std::vector<module_session> module_manager::get_sessions() const
{
    std::vector<module_session> sessions;
    for (const auto& session : sessions_)
    {
        sessions.push_back(session.second);
    }
    return sessions;
}

void module_manager::run_(const settings::s_manager params)
{
    logger_->info("Worker thread started");
    auto last_look_up = boost::chrono::system_clock::now();
    auto interval_look_up = params.interval.unhealthy;
    while (running_.load())
    {
        {
            boost::unique_lock lock(mutex_);
            condition_.wait_for(lock, interval_look_up, [this]
            {
                return !running_.load() || !tasks_queue_.empty();
            });
            if (!running_.load()) break;
            if (!tasks_queue_.empty())
            {
                lock.unlock();
                tasks_queue_.front()();
                tasks_queue_.pop();
            }
        }

        auto now = boost::chrono::system_clock::now();
        if (now - last_look_up > interval_look_up)
        {
            interval_look_up = look_up_() ? params.interval.healthy : params.interval.unhealthy;
            last_look_up = now;
        }
    }

    logger_->info("Unloading all modules...");
    int sleep_time = 200;
    while (true)
    {
        bool unloaded_all_modules = true;

        for (const auto& session : sessions_)
        {
            i_module* module = session.second.module;
            module_status last_status = module->get_last_status();
            if (!last_status.is_processed())
            {
                logger_->warn("Module '{}' is still {} -- {}, waiting...", module->name(), last_status.get_state(), last_status.get_message());
                unloaded_all_modules = false;
                continue;
            }
            if (module->loaded())
            {
                try
                {
                    logger_->info("Unloading module: {}", module->name());
                    bool unloaded = module->unload();
                    if (unloaded)
                        logger_->info("[-] Unloaded module: '{}'", module->name());
                    else
                        logger_->error("[-] Failed to unload module: '{}'", module->name());
                }
                catch (std::exception& e)
                {
                    logger_->error("[-] Exception while unloading '{}': {}", module->name(), e.what());
                }
            }
        }

        if (unloaded_all_modules)
        {
            logger_->info("All modules unloaded. Worker thread stopped");
            break;
        }
        logger_->info("Sleep for {} milliseconds before next cleanup...", sleep_time);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(sleep_time));
    }
}

bool module_manager::look_up_()
{
    logger_->debug("--- Look up ---");
    std::unordered_set<std::string> unhealthy_modules;
    for (auto& [moduleName, session] : sessions_)
    {
        if (!running_.load())
        {
            logger_->warn("Module manager has been stopped during look up modules"
                ".");
            break;
        }
        const module_description& description = module_cast(session.params);
        if (session.checker->check() == health_status::healthy)
        {
            if (!ready_by_dependency(session))
            {
                logger_->warn("'{}' is healthy but depends on unhealthy modules — unloading", description.module_name);
                session.module->unload();
            }
            else
            {
                if (is_running_with_same_params_(session)) continue;
                if (try_reload_(session))
                {
                    session.params = session.module->get_params();
                    logger_->info("'{}' reloaded with new params", description.module_name);
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
            logger_->error("'{}' is unhealthy", moduleName);
            unhealthy_modules.insert(moduleName);
            continue;
        }
        extra_(session);
    }
    if (unhealthy_modules.empty())
        logger_->info("All modules healthy");
    else
        logger_->error("Unhealthy: [{}]", absl::StrJoin(unhealthy_modules, ", "));
    return unhealthy_modules.empty();
}

std::vector<std::string> module_manager::get_unhealthy_dependent_modules_by_order(const module_session& session)
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

bool module_manager::ready_by_dependency(const module_session& session)
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

bool module_manager::ready_by_order(const module_session& session)
{
    return get_unhealthy_dependent_modules_by_order(session).empty();
}

bool module_manager::try_load_(const module_session& session)
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
        logger_->error("Cannot load '{}': dependencies not ready: [{}]",
                       description.module_name, absl::StrJoin(unhealthy_dependent_modules, ", "));
        return false;
    }

    std::vector<std::string> unhealthy_dependent_modules_by_order = get_unhealthy_dependent_modules_by_order(session);
    if (!unhealthy_dependent_modules.empty())
    {
        logger_->error("Cannot load '{}': load-order dependencies not ready: [{}]",
                       description.module_name, absl::StrJoin(unhealthy_dependent_modules_by_order, ", "));
        return false;
    }

    if (!session.module->load(session.params))
    {
        logger_->error("Load failed: '{}'", description.module_name);
        return false;
    }

    logger_->info("[+] Loaded module: '{}'", description.module_name);
    return true;
}

bool module_manager::try_reload_(const module_session& session)
{
    return session.module->reload(session.params);
}

void module_manager::extra_(const module_session& session)
{
    const module_description& description = module_cast(session.params);

    if (description.module_name == "loopback")
    {
        auto loopback_module = dynamic_cast<i_audio_loopback_module*>(session.module);

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
        auto loopback_module = dynamic_cast<i_audio_loopback_module*>(session.module);
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

bool module_manager::is_running_with_same_params_(const module_session& session)
{
    return session.module->get_params() == session.params;
}
