//
// Created by bimba on 11/19/25.
//

#include "AudioRouterModule.hpp"

#include <iostream>

#include "../../logger/LoggerFactory.hpp"
#include "../../stream/FIFOAudioSinkStream.hpp"
#include "../../stream/PortAudioSourceStream.hpp"

AudioRouterModule::AudioRouterModule(const Services* services):
    services(services)
{
    if (services)
    {
        if (services->has<LoggerFactory>()) logger_ = services->get<LoggerFactory>()->create("AudioRouterModule");
    }
}

AudioRouterModule::~AudioRouterModule()
{
    try
    {
        logger_->info("Destruct");
        if (AudioRouterModule::loaded())AudioRouterModule::unload();
        if (services)
        {
            if (services->has<LoggerFactory>()) services->get<LoggerFactory>()->drop(logger_);
        }
    }
    catch (...)
    {
        std::cerr << "Exception caught on ~AudioRouterModule! " << std::endl;
    }
}

bool AudioRouterModule::load(const ModuleParams& moduleParams)
{
    logger_->info("Request to load");
    if (!std::holds_alternative<settings::s_module::s_router>(moduleParams))
        throw std::runtime_error("Unsupported type of parameters passed to AudioRouterModule::load");
    if (loaded())
        throw std::runtime_error("AudioRouterModule already loaded");
    settings::s_module::s_router params = std::get<settings::s_module::s_router>(moduleParams);
    AudioStreamParameters sourceParameters;
    sourceParameters.name = params.source.name;
    sourceParameters.channels = params.source.channels;
    sourceParameters.framesPerBuffer = params.source.framesPerBuffer;
    sourceParameters.sampleRate = params.source.sampleRate;
    std::unique_ptr<IAudioSourceStream> sourceStream = std::make_unique<PortAudioSourceStream>(services, "router");
    if (!sourceStream->start(sourceParameters))
    {
        logger_->error("Failed to start audio source stream");
        return false;
    }
    AudioStreamParameters sinkParameters;
    sinkParameters.name = params.sink.name;
    sinkParameters.channels = params.sink.channels;
    sinkParameters.framesPerBuffer = params.sink.framesPerBuffer;
    sinkParameters.sampleRate = params.sink.sampleRate;
    std::unique_ptr<IAudioSinkStream> sinkStream = std::make_unique<FIFOAudioSinkStream>(services, "router");
    if (!sinkStream->start(sinkParameters))
    {
        logger_->error("Failed to start audio sink stream");
        sourceStream->stop();
        return false;
    }
    sourceStream->addSink(sinkStream.get());
    params_ = std::move(params);
    sinkStream_ = std::move(sinkStream);
    sourceStream_ = std::move(sourceStream);
    return true;
}

bool AudioRouterModule::reload(const ModuleParams& moduleParams)
{
    if (!std::holds_alternative<settings::s_module::s_router>(moduleParams))
        throw std::runtime_error("Unsupported type of parameters passed to AudioRouterModule::load");
    if (!loaded())
        throw std::runtime_error("AudioRouterModule not loaded to reload");
    settings::s_module::s_router params = std::get<settings::s_module::s_router>(moduleParams);
    if (params == params_)
    {
        logger_->warn("Passed same parameters to reload. Reload is pointless");
        return false;
    }
    if (!unload())
    {
        logger_->error("AudioRouterModule reload failed, failed to unload");
        return false;
    }
    return load(params);
}

bool AudioRouterModule::unload()
{
    if (!loaded())
        throw std::runtime_error("AudioRouterModule not loaded to unload");
    if (sourceStream_->stop())
        logger_->info("Successfully stopped audio source stream");
    else
        logger_->error("Failed to stop audio source stream");
    if (sinkStream_->stop())
        logger_->info("Successfully stopped audio sink stream");
    else
        logger_->error("Failed to stop audio sink stream");
    sourceStream_.reset();
    sinkStream_.reset();
    return true;
}

bool AudioRouterModule::loaded() const
{
    return sourceStream_ != nullptr && sinkStream_ != nullptr &&
        sourceStream_->started() && sinkStream_->started();
}

std::string AudioRouterModule::name() const
{
    return "router";
}

ModuleParams AudioRouterModule::getParams() const
{
    if (!loaded()) throw std::runtime_error("getParams is not loaded");
    return params_;
}
