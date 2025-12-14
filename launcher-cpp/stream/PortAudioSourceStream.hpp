//
// Created by bimba on 11/20/25.
//

#ifndef PORTAUDIOSOURCESTREAM_HPP
#define PORTAUDIOSOURCESTREAM_HPP
#include "IAudioSourceStream.hpp"
#include "portaudio.h"


class PortAudioSourceStream : public IAudioSourceStream
{
    static int callback_(
        const void* input, void* output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);

public:
    explicit PortAudioSourceStream(const Services* services, const std::string& id);
    ~PortAudioSourceStream() override;
    bool start(const AudioStreamParameters& parameters) override;
    bool stop() override;
    bool started() override;
    void send(const AudioChunk& chunk) override;

private:
    int callback_(
        const void* input, void* output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);

    PaStream* m_pStream;
    AudioStreamParameters params_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //PORTAUDIOSOURCESTREAM_HPP
