//
// Created by bimba on 11/21/25.
//

#include "fifo_audio_sink_stream.hpp"


fifo_audio_sink_stream::fifo_audio_sink_stream(const services* services, const std::string& id)
    : i_audio_sink_stream(services, id),
      fifo_({})
{
    if (m_services)
    {
        if (m_services->has<logger_factory>())
            logger_ = m_services->get<logger_factory>()->create(
                "FIFOAudioSinkStream-" + id);
    }
}

fifo_audio_sink_stream::~fifo_audio_sink_stream()
{
    if (m_services)
    {
        if (m_services->has<logger_factory>()) m_services->get<logger_factory>()->drop(logger_);
    }
}

audio_stream_result fifo_audio_sink_stream::start(const audio_stream_params& parameters)
{
    if (started()) throw std::runtime_error("FIFOAudioSinkStream already started");
    logger_->info("Request to open {}", parameters.name);
    std::unique_lock lock(mutex_);
    std::ofstream fifo;
    fifo.open(parameters.name);
    if (!fifo.is_open())
    {
        return audio_stream_result::failed(absl::StrFormat("Failed to open: %s", parameters.name));
    }
    logger_->info("Successfully opened {}", parameters.name);
    params_ = parameters;
    fifo_ = std::move(fifo);
    return audio_stream_result::ok();
}

audio_stream_result fifo_audio_sink_stream::stop()
{
    if (!started()) throw std::runtime_error("FIFOAudioSinkStream not started");
    std::unique_lock lock(mutex_);
    fifo_.close();
    return fifo_.is_open() ? audio_stream_result::failed(absl::StrFormat("Failed to close: %s", params_.name)) : audio_stream_result::ok();
}

bool fifo_audio_sink_stream::started()
{
    return fifo_.is_open();
}

void fifo_audio_sink_stream::receive(const audio_chunk& chunk)
{
    if (fifo_.is_open())
    {
        logger_->debug("Write chunk to {}", params_.name);

        fifo_.write(static_cast<const char*>(chunk.data), chunk.bytes());
    }
    else
    {
        logger_->error("Failed to write chunk to {}, pipe closed", params_.name);
    }
}
