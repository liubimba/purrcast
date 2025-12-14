//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOSTREAM_HPP
#define IAUDIOSTREAM_HPP

#include <string>
#include <utility>

#include "../services/Services.hpp"

struct AudioStreamParameters
{
    std::string name;
    int framesPerBuffer = 512;
    int sampleRate = 48000;
    int channels = 0;
};

class IAudioStream
{
public:
    explicit IAudioStream(const Services* services, std::string id):
        m_services(services), m_id(std::move(id))
    {
    }

    virtual ~IAudioStream() = default;

    virtual bool start(const AudioStreamParameters& parameters) = 0;
    virtual bool stop() = 0;
    virtual bool started() = 0;


    [[nodiscard]] virtual std::string getId() const
    {
        return m_id;
    }

protected:
    std::string m_id;
    const Services* m_services;
};

#endif //IAUDIOSTREAM_HPP
