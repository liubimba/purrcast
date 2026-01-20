//
// Created by bimba on 11/23/25.
//

#ifndef IAUDIOLOOPBACKMODULE_HPP
#define IAUDIOLOOPBACKMODULE_HPP
#include "audio_device_info.hpp"
#include "../IModule.hpp"

class IAudioLoopbackModule : public IModule
{
public:
    ~IAudioLoopbackModule() = default;

    virtual std::string get_origin_alsa_device() = 0;
    virtual std::string get_loopback_alsa_device() = 0;
    virtual std::string get_loopback_monitor_description() = 0;
    virtual std::string get_loopback_description() = 0;

    virtual audio_device get_origin_device() = 0;
    virtual audio_device get_loopback_device() = 0;
    virtual audio_device get_loopback_monitor_device() = 0;
};

#endif //IAUDIOLOOPBACKMODULE_HPP
