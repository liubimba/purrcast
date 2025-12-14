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
    AudioLoopbackModule(const Services* services);
    ~AudioLoopbackModule() override;
    bool load(const ModuleParams& params) override;
    bool reload(const ModuleParams& params) override;
    bool unload() override;
    bool loaded() const override;
    std::string name() const override;
    ModuleParams getParams() const override;
    uint32_t getSinkIndex() override;
    std::string getMonitorDescription() override;

private:
    const Services* services_;
    std::unique_ptr<IAudioLoopbackModule> internalModule_;
};


#endif //AUDIOLOOPBACKMODULE_HPP
