//
// Created by bimba on 11/19/25.
//

#ifndef AUDIOLOOPBACKMODULE_HPP
#define AUDIOLOOPBACKMODULE_HPP
#include "i_audio_loopback_module.hpp"
#include "../i_module.hpp"
#include "../../services/services.hpp"
#include "pulse_audio_loopback_module.hpp"

class audio_loopback_module : public i_audio_loopback_module
{
public:
    explicit audio_loopback_module(const services* services, const std::string& id = "");
    ~audio_loopback_module() override;
    bool load(const module_params& params) override;
    bool reload(const module_params& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] module_params get_params() const override;
    audio_device get_origin_device() override;
    audio_device get_loopback_device() override;
    audio_device get_loopback_monitor_device() override;
    [[nodiscard]] module_status get_last_status() const override;

private:
    const services* services_;
    std::unique_ptr<i_audio_loopback_module> internal_;
};


#endif //AUDIOLOOPBACKMODULE_HPP
