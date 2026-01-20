//
// Created by bimba on 12/26/25.
//

#include "TestEnvironmentModule.hpp"


TestEnvironmentModule::TestEnvironmentModule(boost::asio::io_context& io_context, const Services* services,
                                             IAudioSource* source_stream):
    audio_inspector_(std::make_unique<FingerprintAudioInspector>()),
    services_(services), source_stream_(source_stream),
    io_context_(io_context)

{
    if (!source_stream)
        throw std::runtime_error("source stream is required for test environment module");
    logger_ = services->get<LoggerFactory>()->create("test-client-module");
    source_listener_ = std::make_unique<ProxyAudioSinkListener>(std::bind(&TestEnvironmentModule::on_source_chunk_, this, std::placeholders::_1));
    snapcast_listener_ = std::make_unique<ProxyAudioSinkListener>(std::bind(&TestEnvironmentModule::on_snapcast_chunk_, this, std::placeholders::_1));
    source_stream_->add_sink(source_listener_.get());
}

void TestEnvironmentModule::on_source_chunk_(const audio_chunk& chunk)
{
    audio_inspector_->add_reference(chunk, sequence_number_++);
}

void TestEnvironmentModule::on_snapcast_chunk_(const audio_chunk& chunk)
{
    audio_inspector_->verify(chunk);
}

bool TestEnvironmentModule::load(const ModuleParams& module_params)
{
    settings::s_module::s_test_environment params = std::get<settings::s_module::s_test_environment>(module_params);
    AudioStreamParameters audio_params;
    audio_params.name = params.loopback_monitor_description;
    loopback_capture_ = std::make_unique<PortAudioSourceStream>(services_, "test-environment");
    if (!loopback_capture_->start(audio_params))
    {
        logger_->error("Failed to start loopback capture");
        unload();
        return false;
    }
    loopback_capture_->add_sink(snapcast_listener_.get());
    audio_inspector_->reset_stats();
    loaded_ = true;
    logger_->info("Completed loading module");
    return loaded_;
}

bool TestEnvironmentModule::reload(const ModuleParams& params)
{
    return load(params);
}

bool TestEnvironmentModule::unload()
{
    if (loopback_capture_ && loopback_capture_->started())
    {
        loopback_capture_->remove_sink(snapcast_listener_.get());
        loopback_capture_->stop();
        loopback_capture_.reset();
    }
    if (source_stream_)
    {
        source_stream_->remove_sink(source_listener_.get());
    }
    loaded_ = false;
    return !loaded_;
}

bool TestEnvironmentModule::loaded() const
{
    return loaded_;
}

std::string TestEnvironmentModule::name() const
{
    return params_.module_name;
}

ModuleParams TestEnvironmentModule::get_params() const
{
    return params_;
}
