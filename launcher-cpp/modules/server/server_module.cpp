//
// Created by bimba on 12/8/25.
//

#include "server_module.hpp"

#include "../../logger/logger_factory.hpp"
#include "../../util/os_port.hpp"

server_module::server_module(const services* services):
        i_module(), services_(services), mutex_({})
{
        logger_ = services_->get<logger_factory>()->create("ServerModule");
}

bool server_module::load(const module_params& module_params)
{
        if (!std::holds_alternative<settings::s_module::s_server>(module_params))
                throw std::invalid_argument("Invalid module settings");
        if (loaded())
                throw std::runtime_error("ServerModule already loaded");
        settings::s_module::s_server params = std::get<settings::s_module::s_server>(module_params);
        set_last_status_(server_module_status::loading(params));
        logger_->info(last_status_.get_message());
        if (os_port::invalid(params.port))
        {
                set_last_status_(server_module_status::invalid_port(params));
                logger_->error(last_status_.get_message());
                return false;
        }
        if (os_port::is_bound(params.port))
        {
                set_last_status_(server_module_status::bound_port(params));
                logger_->error(last_status_.get_message());
                return false;
        }
        std::unique_lock lock(mutex_);
        std::unique_ptr<os_process> process = std::make_unique<os_process>(services_, name());
        std::string args = absl::StrFormat("--port %d --static_dir %s", params.port, params.static_dir);
        process_result result = process->execute(params.bin, args);
        if (result.is_ok)
        {
                set_last_status_(server_module_status::loaded(params));
                logger_->info(last_status_.get_message());
                params_ = std::move(params);
                server_process_ = std::move(process);
                return true;
        }

        set_last_status_(server_module_status::failed(params, result));
        logger_->error(last_status_.get_message());
        return false;
}

bool server_module::reload(const module_params& moduleParams)
{
        logger_->info("Request to reload");
        if (!std::holds_alternative<settings::s_module::s_server>(moduleParams))
                throw std::runtime_error("SnapclientModule requires settings::module::snapclient type parameters");
        if (!loaded())
                throw std::runtime_error("SnapclientModule is not loaded to request reload it");
        settings::s_module::s_server params = std::get<settings::s_module::s_server>(moduleParams);
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
        return load(moduleParams);
}

bool server_module::unload()
{
        logger_->info("Request to unload");
        if (!loaded())
                throw std::runtime_error("SnapclientModule already loaded to unload");
        std::unique_lock lock(mutex_);
        set_last_status_(module_status::unloading());
        process_result result = server_process_->terminate();
        if (result.is_ok)
        {
                set_last_status_(module_status::unloaded());
                logger_->info(last_status_.get_message());
        }
        else
        {
                set_last_status_(server_module_status::unloaded_with_failures(result));
                logger_->error(last_status_.get_message());
        }
        server_process_.reset();
        return true;
}

bool server_module::loaded() const
{
        return server_process_ != nullptr && server_process_->state() == process_state::RUNNING;
}

std::string server_module::name() const
{
        return "server";
}

module_params server_module::get_params() const
{
        return params_;
}
