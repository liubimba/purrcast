//
// Created by bimba on 11/21/25.
//

#include "FIFOAudioSinkStream.hpp"


FIFOAudioSinkStream::FIFOAudioSinkStream(const Services* services, const std::string& id)
    : IAudioSinkStream(services, id),
      fifo_({})
{
    if (m_services)
    {
        if (m_services->has<LoggerFactory>())
            logger_ = m_services->get<LoggerFactory>()->create(
                "FIFOAudioSinkStream-" + id);
    }
}

FIFOAudioSinkStream::~FIFOAudioSinkStream()
{
    if (m_services)
    {
        if (m_services->has<LoggerFactory>()) m_services->get<LoggerFactory>()->drop(logger_);
    }
}

bool FIFOAudioSinkStream::start(const AudioStreamParameters& parameters)
{
    if (started()) throw std::runtime_error("FIFOAudioSinkStream already started");
    logger_->info("Request to open {}", parameters.name);
    std::unique_lock lock(mutex_);
    std::ofstream fifo;
    fifo.open(parameters.name);
    if (!fifo.is_open())
    {
        logger_->error("Failed to open {}", parameters.name);
        return false;
    }
    logger_->info("Successfully opened {}", parameters.name);
    params_ = parameters;
    fifo_ = std::move(fifo);
    return true;
}

bool FIFOAudioSinkStream::stop()
{
    if (!started()) throw std::runtime_error("FIFOAudioSinkStream not started");
    std::unique_lock lock(mutex_);
    logger_->info("Request to close {}", params_.name);
    fifo_.close();
    return !fifo_.is_open();
}

bool FIFOAudioSinkStream::started()
{
    return fifo_.is_open();
}

void FIFOAudioSinkStream::receive(const AudioChunk& chunk)
{
    if (fifo_.is_open())
    {
        logger_->debug("Write chunk to {}", params_.name);

        fifo_.write(reinterpret_cast<const char*>(chunk.data), chunk.size);
    }
    else
    {
        logger_->error("Failed to write chunk to {}, pipe closed", params_.name);
    }
}
