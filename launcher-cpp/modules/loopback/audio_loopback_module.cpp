//
// Created by bimba on 11/19/25.
//

#include "audio_loopback_module.hpp"


audio_loopback_module::audio_loopback_module(const services* services, const std::string& id):
    services_(services)
{
#if defined(__linux__)
    internal_ = std::make_unique<pulse::audio_loopback_module>(services, id.empty() ? "pulse" : id);
#else
#error unsupported platform for AudioLoopbackModule
#endif
}

audio_loopback_module::~audio_loopback_module() = default;

bool audio_loopback_module::load(const module_params& params)
{
    return internal_->load(params);
}

bool audio_loopback_module::reload(const module_params& params)
{
    return internal_->reload(params);
}

bool audio_loopback_module::unload()
{
    return internal_->unload();
}

bool audio_loopback_module::loaded() const
{
    return internal_->loaded();
}

std::string audio_loopback_module::name() const
{
    return internal_->name();
}

module_params audio_loopback_module::get_params() const
{
    return internal_->get_params();
}

audio_device audio_loopback_module::get_origin_device()
{
    return internal_->get_origin_device();
}

audio_device audio_loopback_module::get_loopback_device()
{
    return internal_->get_loopback_device();
}

audio_device audio_loopback_module::get_loopback_monitor_device()
{
    return internal_->get_loopback_monitor_device();
}

module_status audio_loopback_module::get_last_status() const
{
    return internal_->get_last_status();
}
