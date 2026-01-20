//
// Created by bimba on 11/16/25.
//

#include "SnapclientModule.hpp"


SnapclientModule::SnapclientModule(const Services* services):
    services_(services), params_(settings::s_module::s_snapclient())
{
    logger_ = services_->get<LoggerFactory>()->create("SnapclientModule");
}

bool SnapclientModule::load(const ModuleParams& moduleParams)
{
    logger_->info("Request to load");
    if (!std::holds_alternative<settings::s_module::s_snapclient>(moduleParams))
        throw std::runtime_error("SnapclientModule requires settings::module::snapclient type parameters");
    if (loaded())
        throw std::runtime_error("SnapclientModule already loaded");
    settings::s_module::s_snapclient params = std::get<settings::s_module::s_snapclient>(moduleParams);
    if (!std::filesystem::exists(params.path_to_binary))
        throw std::runtime_error("SnapclientModule binary file does not exists");
    std::string soundcard = absl::StrFormat("--soundcard %s --player pulse", params.soundcard);
    std::unique_lock lock(mutex_);
    std::unique_ptr<Process> process = std::make_unique<Process>(services_, logger_->name());
    if (process->execute(params.path_to_binary, soundcard))
    {
        logger_->info("Successfully executed Snapclient process");
        p_snapclientProcess = std::move(process);
        params_ = params;
        return true;
    }
    logger_->error("Failed to execute Snapclient process");
    return false;
}

bool SnapclientModule::reload(const ModuleParams& moduleParams)
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

bool SnapclientModule::unload()
{
    logger_->info("Request to unload");
    if (!loaded())
        throw std::runtime_error("SnapclientModule is not loaded to unload");
    std::unique_lock lock(mutex_);
    if (p_snapclientProcess->terminate())
    {
        logger_->info("Successfully unloaded module");
        p_snapclientProcess.reset();
        return true;
    }
    logger_->error("Failed to terminate process");
    return false;
}

bool SnapclientModule::loaded() const
{
    return p_snapclientProcess && p_snapclientProcess->state() == ProcessState::RUNNING;
}

std::string SnapclientModule::name() const
{
    return "snapclient";
}

ModuleParams SnapclientModule::get_params() const
{
    if (!loaded()) throw std::runtime_error("getParams is not loaded");
    return params_;
}

PcmDevice SnapclientModule::resolveAudioDeviceName_(const std::string& sinkIndex)
{
    PcmDevice device;
#if defined(__linux__)
    void **hints, **n;
    char *name, *io;
    if (snd_device_name_hint(-1, "pcm", &hints) < 0)
        return device;
    n = hints;
    Alsa::PcmDevice alsaDevice;
    while (*n != nullptr)
    {
        name = snd_device_name_get_hint(*n, "NAME");
        io = snd_device_name_get_hint(*n, "IOID");
        if ((io == nullptr || strcmp(io, "Output") == 0) && (name != nullptr && strstr(name, "DEV=") != nullptr))
        {
            std::string index = strstr(name, "DEV=") + 4;
            if (index == sinkIndex)
            {
                if (std::strstr(name, "hw") == name)
                {
                    alsaDevice.index = index;
                    alsaDevice.name = name;
                    alsaDevice.accessLevel = "hw";
                    break;
                }
                if (std::strstr(name, "plughw") == name)
                {
                    alsaDevice.index = index;
                    alsaDevice.name = name;
                    alsaDevice.accessLevel = "plughw";
                }
                else if (std::strstr(name, "dmix") == name && alsaDevice.accessLevel != "plughw")
                {
                    alsaDevice.index = index;
                    alsaDevice.name = name;
                    alsaDevice.accessLevel = "dmix";
                }
            }
        }
        if (name != nullptr)
            free(name);
        if (io != nullptr)
            free(io);
        n++;
    }
    snd_device_name_free_hint(hints);
    device.index = alsaDevice.index;
    device.name = alsaDevice.name;
#endif
    return device;
}
