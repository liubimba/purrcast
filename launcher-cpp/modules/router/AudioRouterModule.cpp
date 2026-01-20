//
// Created by bimba on 11/19/25.
//

#include "AudioRouterModule.hpp"

#include <iostream>

#include "../../logger/LoggerFactory.hpp"
#include "../../stream/FIFOAudioSinkStream.hpp"
#include "../../stream/FileAudioSinkStream.hpp"
#include "../../stream/IAudioSinkStream.hpp"
#include "../../stream/PortAudioSourceStream.hpp"

AudioRouterModule::AudioRouterModule(const Services* services):
    services(services),
    sink_stream_(std::make_unique<FIFOAudioSinkStream>(services, "router")),
    source_stream_(std::make_unique<PortAudioSourceStream>(services, "router"))
{
    if (services)
    {
        if (services->has<LoggerFactory>()) logger_ = services->get<LoggerFactory>()->create("AudioRouterModule");
    }
    source_stream_->add_sink(sink_stream_.get());
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
    AudioStreamParameters source_params;
    source_params.name = params.source.name;
    source_params.channels = params.source.channels;
    source_params.framesPerBuffer = params.source.framesPerBuffer;
    source_params.sampleRate = params.source.sampleRate;
    if (!source_stream_->start(source_params))
    {
        logger_->error("Failed to start audio source stream");
        return false;
    }
    AudioStreamParameters sink_params;
    sink_params.name = params.sink.name;
    sink_params.channels = params.sink.channels;
    sink_params.framesPerBuffer = params.sink.framesPerBuffer;
    sink_params.sampleRate = params.sink.sampleRate;
    if (!sink_stream_->start(sink_params))
    {
        logger_->error("Failed to start audio sink stream");
        source_stream_->stop();
        return false;
    }

    if (params.record_source)
    {
        std::unique_ptr<IAudioSinkStream> file_stream = std::make_unique<FileAudioSinkStream>(services, "router");
        AudioStreamParameters file_parameters;
        file_parameters.name = "/home/bimba/Projects/multiroom/artifacts/record.pcm";
        if (file_stream->start(file_parameters))
        {
            logger_->info("Monitor of loopback will be recorded into: {}", file_parameters.name);
            source_stream_->add_sink(file_stream.get());
            file_stream_ = std::move(file_stream);
        }
        else
        {
            logger_->error("Failed to open file: {} to record monitor of loopback", file_parameters.name);
        }
    }
    params_ = params;
    std::for_each(sinks_.begin(), sinks_.end(), [this](IAudioSink* sink)
    {
        source_stream_->add_sink(sink);
    });
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
    if (source_stream_->stop())
        logger_->info("Successfully stopped audio source stream");
    else
        logger_->error("Failed to stop audio source stream");
    if (sink_stream_->stop())
        logger_->info("Successfully stopped audio sink stream");
    else
        logger_->error("Failed to stop audio sink stream");
    source_stream_.reset();
    sink_stream_.reset();
    return true;
}

bool AudioRouterModule::loaded() const
{
    return source_stream_ != nullptr && sink_stream_ != nullptr &&
        source_stream_->started() && sink_stream_->started();
}

std::string AudioRouterModule::name() const
{
    return "router";
}

ModuleParams AudioRouterModule::get_params() const
{
    if (!loaded()) throw std::runtime_error("getParams is not loaded");
    return params_;
}

IAudioSource* AudioRouterModule::get_source() const
{
    if (source_stream_)
    {
        return source_stream_.get();
    }
    return nullptr;
}
