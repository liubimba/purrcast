//
// Created by bimba on 11/20/25.
//

#include "port_audio_source_stream.hpp"

#include <pulse/def.h>

#include "../logger/logger_factory.hpp"
#include "../util/os_port.hpp"
#include "absl/strings/str_format.h"


int port_audio_source_stream::callback_(const void* input, void* output, unsigned long frameCount,
                                        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                                        void* userData)
{
    if (userData == nullptr) return paComplete;
    auto self = static_cast<port_audio_source_stream*>(userData);
    return self->callback_(input, output, frameCount, timeInfo, statusFlags);
}

port_audio_source_stream::port_audio_source_stream(const services* services,
                                                   const std::string& id):
    i_audio_source_stream(services, id),
    m_pStream(nullptr)

{
    if (services && services->has<logger_factory>())
        logger_ = services->get<logger_factory>()->create("pa-source-stream-" + id);
}

port_audio_source_stream::~port_audio_source_stream()
{
    logger_->info("Destruct");
    if (m_services)
    {
        if (m_services->has<logger_factory>()) m_services->get<logger_factory>()->drop(logger_);
    }
    try
    {
        if (port_audio_source_stream::started()) port_audio_source_stream::stop();
    }
    catch (...)
    {
    }
}

audio_stream_result port_audio_source_stream::start(const audio_stream_params& parameters)
{
    if (started()) throw std::runtime_error("PortAudioSourceStream already started");
    std::unique_lock lock(mutex_);
    PaError error = Pa_Initialize();
    if (error != paNoError)
    {
        return audio_stream_result::failed(absl::StrFormat("Failed to initialize PortAudio. Error: %s", Pa_GetErrorText(error)));
    }
    PaStreamParameters inputParameters;
    std::memset(&inputParameters, 0, sizeof(inputParameters));
    inputParameters.device = paNoDevice;
    for (PaDeviceIndex index = 0; index < Pa_GetDeviceCount(); ++index)
    {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(index);
        if (deviceInfo->name == parameters.name)
        {
            inputParameters.device = index;
            inputParameters.channelCount =
                parameters.channels <= 0 ? deviceInfo->maxInputChannels : parameters.channels;
            inputParameters.sampleFormat = paInt16;
            inputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
        }
    }
    if (inputParameters.device == paNoDevice)
    {
        return audio_stream_result::failed(absl::StrFormat("No input devices found for: %s", parameters.name));
    }
    logger_->info("Open stream for {}, channels={}, sampleRate={}, framesPerBuffer={}", parameters.name,
                  inputParameters.channelCount, parameters.sampleRate, parameters.framesPerBuffer);
    error = Pa_OpenStream(&m_pStream, &inputParameters, nullptr, parameters.sampleRate, parameters.framesPerBuffer,
                          paNoFlag, callback_, this);
    if (error != paNoError)
    {
        return audio_stream_result::failed(absl::StrFormat("Failed to open input stream for %s. Error: %s", parameters.name, Pa_GetErrorText(error)));
    }
    error = Pa_StartStream(m_pStream);
    if (error != paNoError)
    {
        return audio_stream_result::failed(absl::StrFormat("Failed to start stream for %s. Error: %s", parameters.name, Pa_GetErrorText(error)));
    }
    params_ = parameters;
    params_.channels = inputParameters.channelCount;
    return audio_stream_result::ok();
}

audio_stream_result port_audio_source_stream::stop()
{
    if (!started()) throw std::runtime_error("PortAudioSourceStream not started");
    audio_stream_result result = audio_stream_result::ok();
    PaError error = Pa_StopStream(m_pStream);
    if (error != paNoError)
    {
        logger_->error("Failed to stop stream {}. Error: {}", params_.name, Pa_GetErrorText(error));
    }
    std::unique_lock lock(mutex_);
    error = Pa_CloseStream(m_pStream);
    if (error != paNoError)
    {
        return audio_stream_result::failed(absl::StrFormat("Failed to stop stream %s. Error: %s", params_.name, Pa_GetErrorText(error)));
    }
    logger_->info("Stopped stream {}", params_.name);
    m_pStream = nullptr;
    params_ = audio_stream_params();
    lock.unlock();
    error = Pa_Terminate();
    if (error != paNoError)
        logger_->error("Failed to release PortAudio");
    return audio_stream_result::ok();
}

bool port_audio_source_stream::started()
{
    return m_pStream && Pa_IsStreamActive(m_pStream);
}

void port_audio_source_stream::send(const audio_chunk& chunk)
{
    std::for_each(sinks_.begin(), sinks_.end(), [chunk](i_audio_sink* sink)
    {
        sink->receive(chunk);
    });
}

int port_audio_source_stream::callback_(const void* input, void* output, unsigned long frames_per_buffer,
                                        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags)
{
    if (!sinks_.empty())
        send(audio_chunk{
            .data = input,
            .channels = params_.channels,
            .frames_per_buffer = frames_per_buffer,
            .bytes_per_sample = sizeof(int16_t),
        });
    return paContinue;
}
