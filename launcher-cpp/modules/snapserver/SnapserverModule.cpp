//
// Created by bimba on 11/15/25.
//

#include "SnapserverModule.hpp"

#include <filesystem>

#include "../../logger/LoggerFactory.hpp"
#include "../../util/Port.hpp"
#include "../../util/Process.hpp"

SnapserverModule::SnapserverModule(const Services* services):
    services_(services), params_(settings::s_module::s_snapserver())
{
    logger_ = services->get<LoggerFactory>()->create("SnapserverModule");
}

SnapserverModule::~SnapserverModule()
{
    services_->get<LoggerFactory>()->drop(logger_);
}

bool SnapserverModule::load(const ModuleParams& moduleParams)
{
    logger_->info("Request to load");
    if (!std::holds_alternative<settings::s_module::s_snapserver>(moduleParams))
        throw std::runtime_error("unsupported module parameters type");
    if (loaded())
        throw std::runtime_error("SnapserverModule loaded already");
    settings::s_module::s_snapserver params = std::get<settings::s_module::s_snapserver>(moduleParams);
    if (params.ports.http == -1 || params.ports.stream == -1 || params.ports.control == -1)
    {
        logger_->error("Bad ports");
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
    std::unique_ptr<Process> p_process = std::make_unique<Process>(services_, "snapserver");
    logger_->info("Executing snapserver process");
    if (p_process->execute(params.bin, args))
    {
        logger_->info("Successfully executed snapserver process");
        p_snapserverProcess_ = std::move(p_process);
        params_ = params;
        return true;
    }
    logger_->error("Failed to execute snapserver process");
    return false;
}

bool SnapserverModule::reload(const ModuleParams& moduleParams)
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

bool SnapserverModule::unload()
{
    logger_->info("Request to unload");
    if (!loaded())
        throw std::runtime_error("SnapserverModule unload is unsupported since it's already unloaded");
    std::unique_lock<std::mutex> lock(mutex_);
    if (p_snapserverProcess_->terminate())
    {
        logger_->info("Request to unload succeed");
        p_snapserverProcess_.reset();
        return true;
    }
    logger_->error("Request failed to unload");
    return false;
}

bool SnapserverModule::loaded() const
{
    if (p_snapserverProcess_)
    {
        return p_snapserverProcess_->state() == ProcessState::RUNNING;
    }
    return false;
}

std::string SnapserverModule::name() const
{
    return "snapserver";
}

ModuleParams SnapserverModule::getParams() const
{
    if (!loaded()) throw std::runtime_error("SnapserverModule getParams is not loaded");
    return params_;
}
