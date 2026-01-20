//
// Created by bimba on 11/19/25.
//

#ifndef AUDIOLOOPBACKMODULE_HPP
#define AUDIOLOOPBACKMODULE_HPP
#include "IAudioLoopbackModule.hpp"
#include "../IModule.hpp"
#include "../../services/Services.hpp"
#include "PulseAudioLoopbackModule.hpp"

class AudioLoopbackModule : public IAudioLoopbackModule
{
public:
    AudioLoopbackModule(const Services* services, const std::string& id = "");
    ~AudioLoopbackModule() override;
    bool load(const ModuleParams& params) override;
    bool reload(const ModuleParams& params) override;
    bool unload() override;
    bool loaded() const override;
    std::string name() const override;
    ModuleParams get_params() const override;
    std::string get_origin_alsa_device() override;
    std::string get_loopback_monitor_description() override;
    std::string get_loopback_alsa_device() override;
    std::string get_loopback_description() override;
    audio_device get_origin_device() override;
    audio_device get_loopback_device() override;
    audio_device get_loopback_monitor_device() override;

private:
    const Services* services_;
    std::unique_ptr<IAudioLoopbackModule> internal_;
};


#endif //AUDIOLOOPBACKMODULE_HPP
