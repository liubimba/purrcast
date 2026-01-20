//
// Created by bimba on 11/19/25.
//

#include "AudioLoopbackModule.hpp"


AudioLoopbackModule::AudioLoopbackModule(const Services* services, const std::string& id):
    services_(services)
{
#if defined(__linux__)
    internal_ = std::make_unique<pulse::AudioLoopbackModule>(services, id.empty() ? "pulse" : id);
#else
#error unsupported platform for AudioLoopbackModule
#endif
}

AudioLoopbackModule::~AudioLoopbackModule()
{
}

bool AudioLoopbackModule::load(const ModuleParams& params)
{
    return internal_->load(params);
}

bool AudioLoopbackModule::reload(const ModuleParams& params)
{
    return internal_->reload(params);
}

bool AudioLoopbackModule::unload()
{
    return internal_->unload();
}

bool AudioLoopbackModule::loaded() const
{
    return internal_->loaded();
}

std::string AudioLoopbackModule::name() const
{
    return internal_->name();
}

ModuleParams AudioLoopbackModule::get_params() const
{
    return internal_->get_params();
}

std::string AudioLoopbackModule::get_origin_alsa_device()
{
    return internal_->get_origin_alsa_device();
}

std::string AudioLoopbackModule::get_loopback_monitor_description()
{
    return internal_->get_loopback_monitor_description();
}

std::string AudioLoopbackModule::get_loopback_alsa_device()
{
    return internal_->get_loopback_alsa_device();
}

std::string AudioLoopbackModule::get_loopback_description()
{
    return internal_->get_loopback_description();
}

audio_device AudioLoopbackModule::get_origin_device()
{
    return internal_->get_origin_device();
}

audio_device AudioLoopbackModule::get_loopback_device()
{
    return internal_->get_loopback_device();
}

audio_device AudioLoopbackModule::get_loopback_monitor_device()
{
    return internal_->get_loopback_monitor_device();
}
