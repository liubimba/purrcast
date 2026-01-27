//
// Created by bimba on 11/19/25.
//

#include "audio_router_module.hpp"

#include <iostream>

#include "../../logger/logger_factory.hpp"
#include "../../stream/fifo_audio_sink_stream.hpp"
#include "../../stream/file_audio_sink_stream.hpp"
#include "../../stream/i_audio_sink_stream.hpp"
#include "../../stream/port_audio_source_stream.hpp"

audio_router_module::audio_router_module(const services* services):
    services_(services),
    sink_stream_(std::make_unique<fifo_audio_sink_stream>(services, "router")),
    source_stream_(std::make_unique<port_audio_source_stream>(services, "router"))
{
    if (services)
    {
        if (services->has<logger_factory>()) logger_ = services->get<logger_factory>()->create("AudioRouterModule");
    }
    source_stream_->add_sink(sink_stream_.get());
}

audio_router_module::~audio_router_module()
{
    try
    {
        logger_->info("Destruct");
        if (audio_router_module::loaded())audio_router_module::unload();
        if (services_)
        {
            if (services_->has<logger_factory>()) services_->get<logger_factory>()->drop(logger_);
        }
    }
    catch (...)
    {
        std::cerr << "Exception caught on ~AudioRouterModule! " << std::endl;
    }
}

bool audio_router_module::load(const module_params& moduleParams)
{
    logger_->info("Request to load");
    if (!std::holds_alternative<settings::s_module::s_router>(moduleParams))
        throw std::runtime_error("Unsupported type of parameters passed to AudioRouterModule::load");
    if (loaded())
        throw std::runtime_error("AudioRouterModule already loaded");
    settings::s_module::s_router params = std::get<settings::s_module::s_router>(moduleParams);
    set_last_status_(router_module_status::loading(params));
    audio_stream_params source_params;
    source_params.name = params.source.name;
    source_params.channels = params.source.channels;
    source_params.framesPerBuffer = params.source.frames_per_buffer;
    source_params.sampleRate = params.source.sample_rate;
    audio_stream_result result = source_stream_->start(source_params);
    if (!result.is_ok)
    {
        set_last_status_(router_module_status::failed_to_start_source_stream(result.message));
        logger_->error(last_status_.get_message());
        return false;
    }
    set_last_status_(router_module_status::started_source_stream());
    audio_stream_params sink_params;
    sink_params.name = params.sink.name;
    sink_params.channels = params.sink.channels;
    sink_params.framesPerBuffer = params.sink.frames_per_buffer;
    sink_params.sampleRate = params.sink.sample_rate;
    result = sink_stream_->start(sink_params);
    if (!result.is_ok)
    {
        set_last_status_(router_module_status::failed_to_start_source_stream(result.message));
        logger_->error(last_status_.get_message());
        source_stream_->stop();
        return false;
    }
    set_last_status_(router_module_status::started_sink_stream());
    if (params.record_source)
    {
        std::unique_ptr<i_audio_sink_stream> file_stream = std::make_unique<file_audio_sink_stream>(services_, "router");
        audio_stream_params file_parameters;
        file_parameters.name = "/home/bimba/Projects/multiroom/artifacts/record.pcm";
        result = file_stream->start(file_parameters);
        if (result.is_ok)
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
    std::for_each(sinks_.begin(), sinks_.end(), [this](i_audio_sink* sink)
    {
        source_stream_->add_sink(sink);
    });
    set_last_status_(router_module_status::loaded(params_));
    return true;
}

bool audio_router_module::reload(const module_params& moduleParams)
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

bool audio_router_module::unload()
{
    if (!loaded())
        throw std::runtime_error("AudioRouterModule not loaded to unload");
    set_last_status_(module_status::unloading());

    audio_stream_result stop_source = source_stream_->stop();
    audio_stream_result stop_sink = sink_stream_->stop();

    set_last_status_(router_module_status::unloaded(stop_source, stop_sink));

    source_stream_.reset();
    sink_stream_.reset();
    return true;
}

bool audio_router_module::loaded() const
{
    return source_stream_ != nullptr && sink_stream_ != nullptr &&
        source_stream_->started() && sink_stream_->started();
}

std::string audio_router_module::name() const
{
    return "router";
}

module_params audio_router_module::get_params() const
{
    if (!loaded()) throw std::runtime_error("getParams is not loaded");
    return params_;
}

IAudioSource* audio_router_module::get_source() const
{
    if (source_stream_)
    {
        return source_stream_.get();
    }
    return nullptr;
}

void audio_router_module::add_sink(i_audio_sink* stream)
{
    sinks_.emplace_back(stream);
}

void audio_router_module::remove_sink(i_audio_sink* stream)
{
    sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), stream), sinks_.end());
}

i_audio_source_stream* audio_router_module::get_source_stream() const
{
    return source_stream_.get();
}
