//
// Created by bimba on 11/23/25.
//

#ifndef IAUDIOLOOPBACKMODULE_HPP
#define IAUDIOLOOPBACKMODULE_HPP
#include "audio_device_info.hpp"
#include "../i_module.hpp"

class i_audio_loopback_module : public i_module
{
public:
    ~i_audio_loopback_module() override = default;

    virtual audio_device get_origin_device() = 0;
    virtual audio_device get_loopback_device() = 0;
    virtual audio_device get_loopback_monitor_device() = 0;
};

#endif //IAUDIOLOOPBACKMODULE_HPP
