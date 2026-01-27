//
// Created by bimba on 11/15/25.
//

#include "snapserver_module.hpp"

#include <filesystem>

#include "../../logger/logger_factory.hpp"
#include "../../util/os_port.hpp"
#include "../../util/os_process.hpp"

snapserver_module::snapserver_module(const services* services):
    services_(services), params_(settings::s_module::s_snapserver())
{
    logger_ = services->get<logger_factory>()->create("SnapserverModule");
}

snapserver_module::~snapserver_module()
{
    services_->get<logger_factory>()->drop(logger_);
}

bool snapserver_module::load(const module_params& moduleParams)
{
    if (!std::holds_alternative<settings::s_module::s_snapserver>(moduleParams))
        throw std::runtime_error("unsupported module parameters type");
    if (loaded())
        throw std::runtime_error("SnapserverModule loaded already");
    settings::s_module::s_snapserver params = std::get<settings::s_module::s_snapserver>(moduleParams);
    if (params.ports.http == -1 || params.ports.stream == -1 || params.ports.control == -1)
    {
        set_last_status_(snapserver_module_status::invalid_ports(params.ports));
        logger_->error(last_status_.get_message());
        return false;
    }
    // if (Port::isBound(params.ports.stream))
    // {
    //     logger_->error("Stream server port {} is bound", params.ports.stream);
    //     return false;
    // }
    // if (Port::isBound(params.ports.http))
    // {
    //     logger_->error("HTTP server port {} is bound", params.ports.http);
    //     return false;
    // }
    // if (Port::isBound(params.ports.control))
    // {
    //     logger_->error("Control server port {} is bound", params.ports.control);
    //     return false;
    // }
    std::string args;
    if (std::filesystem::exists(params.config))
    {
        args = absl::StrFormat("--config %s", params.config);
    }
    std::unique_lock lock(mutex_);
    std::unique_ptr<os_process> p_process = std::make_unique<os_process>(services_, "snapserver");
    set_last_status_(snapserver_module_status::loading(params.path_to_binary, args));
    process_result result = p_process->execute(params.path_to_binary, args);
    if (result.is_ok)
    {
        set_last_status_(snapserver_module_status::loaded(params.path_to_binary, args));
        logger_->info(last_status_.get_message());
        params_ = std::move(params);
        snapserver_process_ = std::move(p_process);
        return true;
    }
    set_last_status_(snapserver_module_status::failed_to_load(result.message));
    logger_->error(last_status_.get_message());
    return false;
}

bool snapserver_module::reload(const module_params& moduleParams)
{
    logger_->info("Request to reload");
    if (!std::holds_alternative<settings::s_module::s_snapserver>(moduleParams))
        throw std::runtime_error("unsupported module parameters type");
    if (!loaded())
        throw std::runtime_error("SnapserverModule reload is unsupported since it's not loaded");
    settings::s_module::s_snapserver params = std::get<settings::s_module::s_snapserver>(moduleParams);
    if (params == params_)
    {
        logger_->warn("Reload is pointless since the passed parameters match the current ones");
        return false;
    }
    if (!unload())
    {
        logger_->warn("Reload is failed, cause failed to unload firstly");
        return false;
    }
    return load(moduleParams);
}

bool snapserver_module::unload()
{
    if (!loaded())
        throw std::runtime_error("SnapserverModule unload is unsupported since it's already unloaded");
    set_last_status_(module_status::unloading());
    std::unique_lock lock(mutex_);
    process_result result = snapserver_process_->terminate();
    if (result.is_ok)
    {
        set_last_status_(module_status::unloaded());
        logger_->info(last_status_.get_message());
    }
    else
    {
        set_last_status_(snapserver_module_status::unloaded_with_errors(result.message));
        logger_->error(last_status_.get_message());
    }
    snapserver_process_.reset();
    return true;
}

bool snapserver_module::loaded() const
{
    if (snapserver_process_)
    {
        return snapserver_process_->state() == process_state::RUNNING;
    }
    return false;
}

std::string snapserver_module::name() const
{
    return "snapserver";
}

module_params snapserver_module::get_params() const
{
    if (!loaded()) throw std::runtime_error("SnapserverModule getParams is not loaded");
    return params_;
}
