//
// Created by bimba on 12/26/25.
//

#include "test_environment.hpp"


test_environment::test_environment(boost::asio::io_context& io_context, const services* services,
                                   IAudioSource* source_stream):
    audio_inspector_(std::make_unique<fingerprint_audio_inspector>()),
    services_(services), source_stream_(source_stream),
    io_context_(io_context)

{
    if (!source_stream)
        throw std::runtime_error("source stream is required for test environment module");
    logger_ = services->get<logger_factory>()->create("test-client-module");
    source_listener_ = std::make_unique<proxy_audio_sink_listener>(std::bind(&test_environment::on_source_chunk_, this, std::placeholders::_1));
    snapcast_listener_ = std::make_unique<proxy_audio_sink_listener>(std::bind(&test_environment::on_snapcast_chunk_, this, std::placeholders::_1));
    source_stream_->add_sink(source_listener_.get());
}

void test_environment::on_source_chunk_(const audio_chunk& chunk)
{
    audio_inspector_->add_reference(chunk, sequence_number_++);
}

void test_environment::on_snapcast_chunk_(const audio_chunk& chunk)
{
    audio_inspector_->verify(chunk);
}

bool test_environment::load(const module_params& module_params)
{
    settings::s_module::s_test_environment params = std::get<settings::s_module::s_test_environment>(module_params);
    set_last_status_(test_environment_module_status::loading(params));
    audio_stream_params audio_params;
    audio_params.name = params.loopback_monitor_description;
    loopback_capture_ = std::make_unique<port_audio_source_stream>(services_, "test-environment");
    audio_stream_result result = loopback_capture_->start(audio_params);
    if (result.is_ok)
    {
        set_last_status_(test_environment_module_status::failed_to_start_loopback(result));
        logger_->error(last_status_.get_message());
        unload();
        return false;
    }
    loopback_capture_->add_sink(snapcast_listener_.get());
    audio_inspector_->reset_stats();
    loaded_ = true;
    set_last_status_(test_environment_module_status::loaded(params));
    logger_->info(last_status_.get_message());
    return loaded_;
}

bool test_environment::reload(const module_params& params)
{
    return load(params);
}

bool test_environment::unload()
{
    if (loopback_capture_ && loopback_capture_->started())
    {
        loopback_capture_->remove_sink(snapcast_listener_.get());
        audio_stream_result result = loopback_capture_->stop();
        loopback_capture_.reset();
        set_last_status_(test_environment_module_status::unloaded(result));
    }
    else
    {
        set_last_status_(test_environment_module_status::unloaded(audio_stream_result::ok()));
    }
    if (source_stream_)
    {
        source_stream_->remove_sink(source_listener_.get());
    }
    loaded_ = false;
    return true;
}

bool test_environment::loaded() const
{
    return loaded_;
}

std::string test_environment::name() const
{
    return params_.module_name;
}

module_params test_environment::get_params() const
{
    return params_;
}
