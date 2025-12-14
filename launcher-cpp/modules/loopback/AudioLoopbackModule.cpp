//
// Created by bimba on 11/19/25.
//

#include "AudioLoopbackModule.hpp"


AudioLoopbackModule::AudioLoopbackModule(const Services* services):
    services_(services)
{
#if defined(__linux__)
    internalModule_ = std::make_unique<pulse::AudioLoopbackModule>(services);
#else
#error unsupported platform for AudioLoopbackModule
#endif
}

AudioLoopbackModule::~AudioLoopbackModule()
{
}

bool AudioLoopbackModule::load(const ModuleParams& params)
{
    return internalModule_->load(params);
}

bool AudioLoopbackModule::reload(const ModuleParams& params)
{
    return internalModule_->reload(params);
}

bool AudioLoopbackModule::unload()
{
    return internalModule_->unload();
}

bool AudioLoopbackModule::loaded() const
{
    return internalModule_->loaded();
}

std::string AudioLoopbackModule::name() const
{
    return internalModule_->name();
}

ModuleParams AudioLoopbackModule::getParams() const
{
    return internalModule_->getParams();
}

uint32_t AudioLoopbackModule::getSinkIndex()
{
    return internalModule_->getSinkIndex();
}

std::string AudioLoopbackModule::getMonitorDescription()
{
    return internalModule_->getMonitorDescription();
}
