//
// Created by bimba on 12/8/25.
//

#include "ServerModule.hpp"

#include "../../logger/LoggerFactory.hpp"
#include "../../util/Port.hpp"

ServerModule::ServerModule(const Services* services):
        IModule(), p_services_(services), mutex_({})
{
        p_logger_ = p_services_->get<LoggerFactory>()->create("ServerModule");
}

bool ServerModule::load(const ModuleParams& moduleParams)
{
        p_logger_->info("Request to load");
        if (!std::holds_alternative<settings::s_module::s_server>(moduleParams))
                throw std::invalid_argument("Invalid module settings");
        if (loaded())
                throw std::runtime_error("ServerModule already loaded");
        settings::s_module::s_server params = std::get<settings::s_module::s_server>(moduleParams);
        if (Port::invalid(params.port))
        {
                p_logger_->error("Invalid port {}", params.port);
                return false;
        }
        if (Port::isBound(params.port))
        {
                p_logger_->error("Port {} is bound", params.port);
                return false;
        }
        std::unique_lock lock(mutex_);
        std::unique_ptr<Process> process = std::make_unique<Process>(p_services_, name());
        std::string args = absl::StrFormat("--port %d", params.port);
        if (process->execute(params.bin, args))
        {
                p_logger_->info("Process started");
                p_process_ = std::move(process);
                params_ = std::move(params);
                return true;
        }
        p_logger_->error("Failed to start process");
        return false;
}

bool ServerModule::reload(const ModuleParams& moduleParams)
{
        p_logger_->info("Request to reload");
        if (!std::holds_alternative<settings::s_module::s_server>(moduleParams))
                throw std::runtime_error("SnapclientModule requires settings::module::snapclient type parameters");
        if (!loaded())
                throw std::runtime_error("SnapclientModule is not loaded to request reload it");
        settings::s_module::s_server params = std::get<settings::s_module::s_server>(moduleParams);
        if (params_ == params)
        {
                p_logger_->info("Passed same parameteres, reloading is pointless");
                return false;
        }
        if (!unload())
        {
                p_logger_->error("Failed to unload module to load it again");
                return false;
        }
        return load(moduleParams);
}

bool ServerModule::unload()
{
        p_logger_->info("Request to unload");
        if (!loaded())
                throw std::runtime_error("SnapclientModule already loaded to unload");
        std::unique_lock lock(mutex_);
        if (p_process_->terminate())
        {
                p_logger_->info("Successfully unloaded module");
                p_process_.reset();
                return true;
        }
        p_logger_->error("Failed to terminate process");
        return false;
}

bool ServerModule::loaded() const
{
        return p_process_ != nullptr && p_process_->state() == ProcessState::RUNNING;
}

std::string ServerModule::name() const
{
        return "server";
}

ModuleParams ServerModule::getParams() const
{
        return params_;
}
