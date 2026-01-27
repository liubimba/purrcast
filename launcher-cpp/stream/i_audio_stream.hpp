//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOSTREAM_HPP
#define IAUDIOSTREAM_HPP

#include <string>
#include <utility>

#include "audio_stream_result.hpp"
#include "../services/services.hpp"

struct audio_stream_params
{
    std::string name;
    int framesPerBuffer = 512;
    int sampleRate = 48000;
    int channels = 0;
};

class i_audio_stream
{
public:
    explicit i_audio_stream(const services* services, std::string id):
        m_services(services), m_id(std::move(id))
    {
    }

    virtual ~i_audio_stream() = default;

    virtual audio_stream_result start(const audio_stream_params& parameters) = 0;
    virtual audio_stream_result stop() = 0;
    virtual bool started() = 0;


    [[nodiscard]] virtual std::string get_id() const
    {
        return m_id;
    }

protected:
    std::string m_id;
    const services* m_services;
};

#endif //IAUDIOSTREAM_HPP
