//
// Created by bimba on 11/20/25.
//

#ifndef PORTAUDIOSOURCESTREAM_HPP
#define PORTAUDIOSOURCESTREAM_HPP
#include "IAudioSource.hpp"
#include "i_audio_source_stream.hpp"
#include "i_audio_stream.hpp"
#include "portaudio.h"

class port_audio_source_stream : public i_audio_source_stream
{
    static int callback_(
        const void* input, void* output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);

public:
    explicit port_audio_source_stream(const services* services, const std::string& id);
    ~port_audio_source_stream() override;
    audio_stream_result start(const audio_stream_params& parameters) override;
    audio_stream_result stop() override;
    bool started() override;
    void send(const audio_chunk& chunk) override;

private:
    int callback_(
        const void* input, void* output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);

    PaStream* m_pStream;
    audio_stream_params params_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //PORTAUDIOSOURCESTREAM_HPP
