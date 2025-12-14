//
// Created by bimba on 11/20/25.
//

#include "PortAudioSourceStream.hpp"

#include <pulse/def.h>

#include "../logger/LoggerFactory.hpp"
#include "../util/Port.hpp"


int PortAudioSourceStream::callback_(const void* input, void* output, unsigned long frameCount,
                                     const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                                     void* userData)
{
    if (userData == nullptr) return paComplete;
    auto self = static_cast<PortAudioSourceStream*>(userData);
    return self->callback_(input, output, frameCount, timeInfo, statusFlags);
}

PortAudioSourceStream::PortAudioSourceStream(const Services* services,
                                             const std::string& id):
    IAudioSourceStream(services, id),
    m_pStream(nullptr)

{
    if (services && services->has<LoggerFactory>())
        logger_ = services->get<LoggerFactory>()->create("PortAudioSourceStream-" + id);
}

PortAudioSourceStream::~PortAudioSourceStream()
{
    logger_->info("Destruct");
    if (m_services)
    {
        if (m_services->has<LoggerFactory>()) m_services->get<LoggerFactory>()->drop(logger_);
    }
    try
    {
        if (PortAudioSourceStream::started()) PortAudioSourceStream::stop();
    }
    catch (...)
    {
    }
}

bool PortAudioSourceStream::start(const AudioStreamParameters& parameters)
{
    logger_->info("Request to start {}", parameters.name);
    if (started()) throw std::runtime_error("PortAudioSourceStream already started");
    std::unique_lock lock(mutex_);
    PaError error = Pa_Initialize();
    if (error != paNoError)
    {
        logger_->error("Failed to initialize PortAudio. Error: {}", Pa_GetErrorText(error));
        return false;
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
        logger_->info("No input devices found for {}", parameters.name);
        return false;
    }
    logger_->info("Open stream for {}, channels={}, sampleRate={}, framesPerBuffer={}", parameters.name,
                  inputParameters.channelCount, parameters.sampleRate, parameters.framesPerBuffer);
    error = Pa_OpenStream(&m_pStream, &inputParameters, nullptr, parameters.sampleRate, parameters.framesPerBuffer,
                          paNoFlag, callback_, this);
    if (error != paNoError)
    {
        logger_->error("Failed to open input stream for {}. Error: {}", parameters.name, Pa_GetErrorText(error));
        return false;
    }
    error = Pa_StartStream(m_pStream);
    if (error != paNoError)
    {
        logger_->error("Failed to start stream for {}. Error: {}", parameters.name, Pa_GetErrorText(error));
        return false;
    }
    logger_->info("Started stream  {}", parameters.name);
    params_ = parameters;
    params_.channels = inputParameters.channelCount;
    return true;
}

bool PortAudioSourceStream::stop()
{
    if (!started()) throw std::runtime_error("PortAudioSourceStream not started");
    logger_->info("Request to stop {}", params_.name);
    PaError error = Pa_StopStream(m_pStream);
    if (error != paNoError)
    {
        logger_->error("Failed to stop stream {}. Error: {}", params_.name, Pa_GetErrorText(error));
    }
    std::unique_lock lock(mutex_);
    error = Pa_CloseStream(m_pStream);
    if (error != paNoError)
    {
        logger_->info("Failed to stop stream {}. Error: {}", params_.name, Pa_GetErrorText(error));
        return false;
    }
    logger_->info("Stopped stream {}", params_.name);
    m_pStream = nullptr;
    params_ = AudioStreamParameters();
    lock.unlock();
    error = Pa_Terminate();
    if (error != paNoError)
        logger_->error("Failed to release PortAudio");
    return true;
}

bool PortAudioSourceStream::started()
{
    return m_pStream && Pa_IsStreamActive(m_pStream);
}

void PortAudioSourceStream::send(const AudioChunk& chunk)
{
    std::for_each(sinks_.begin(), sinks_.end(), [chunk](IAudioSinkStream* sink)
    {
        sink->receive(chunk);
    });
}

int PortAudioSourceStream::callback_(const void* input, void* output, unsigned long frameCount,
                                     const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags)
{
    if (!sinks_.empty())
        send(AudioChunk{.data = input, .size = frameCount * params_.channels * 2});
    return paContinue;
}
