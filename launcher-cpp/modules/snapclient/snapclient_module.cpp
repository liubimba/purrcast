//
// Created by bimba on 11/16/25.
//

#include "snapclient_module.hpp"

snapclient_module::snapclient_module(const services* services):
    services_(services), params_(settings::s_module::s_snapclient())
{
    logger_ = services_->get<logger_factory>()->create("SnapclientModule");
}

bool snapclient_module::load(const module_params& moduleParams)
{
    if (!std::holds_alternative<settings::s_module::s_snapclient>(moduleParams))
        throw std::runtime_error("SnapclientModule requires settings::module::snapclient type parameters");
    if (loaded())
        throw std::runtime_error("SnapclientModule already loaded");
    settings::s_module::s_snapclient params = std::get<settings::s_module::s_snapclient>(moduleParams);
    set_last_status_(snapclient_module_status::loading(params));
    if (!std::filesystem::exists(params.path_to_binary))
    {
        set_last_status_(snapclient_module_status::binary_file_not_found(params.path_to_binary));
        logger_->error(last_status_.get_message());
        return false;
    }
    std::string soundcard = absl::StrFormat("--soundcard %s --player pulse", params.soundcard);
    std::unique_lock lock(mutex_);
    std::unique_ptr<os_process> process = std::make_unique<os_process>(services_, logger_->name());
    process_result result = process->execute(params.path_to_binary, soundcard);
    if (result.is_ok)
    {
        set_last_status_(snapclient_module_status::loaded(params.path_to_binary));
        params_ = std::move(params);
        snapclient_process_ = std::move(process);
        logger_->info(last_status_.get_message());
        return true;
    }
    set_last_status_(snapclient_module_status::failed_to_execute_binary(params.path_to_binary));
    logger_->error(last_status_.get_message());
    return false;
}

bool snapclient_module::reload(const module_params& moduleParams)
{
    logger_->info("Request to reload");
    if (!std::holds_alternative<settings::s_module::s_snapclient>(moduleParams))
        throw std::runtime_error("SnapclientModule requires settings::module::snapclient type parameters");
    if (!loaded())
        throw std::runtime_error("SnapclientModule is not loaded to request reload it");
    settings::s_module::s_snapclient params = std::get<settings::s_module::s_snapclient>(moduleParams);
    if (params_ == params)
    {
        logger_->info("Passed same parameteres, reloading is pointless");
        return false;
    }
    if (!unload())
    {
        logger_->error("Failed to unload module to load it again");
        return false;
    }
    return load(params);
}

bool snapclient_module::unload()
{
    if (!loaded())
        throw std::runtime_error("SnapclientModule is not loaded to unload");
    set_last_status_(module_status::unloading());
    std::unique_lock lock(mutex_);
    process_result result = snapclient_process_->terminate();
    if (result.is_ok)
    {
        set_last_status_(module_status::unloaded());
        logger_->info(last_status_.to_string());
        snapclient_process_.reset();
        return true;
    }
    set_last_status_(snapclient_module_status::failed_to_terminate_process(result.message));
    logger_->error(last_status_.to_string());
    return false;
}

bool snapclient_module::loaded() const
{
    return snapclient_process_ && snapclient_process_->state() == process_state::RUNNING;
}

std::string snapclient_module::name() const
{
    return "snapclient";
}

module_params snapclient_module::get_params() const
{
    if (!loaded()) throw std::runtime_error("getParams is not loaded");
    return params_;
}
