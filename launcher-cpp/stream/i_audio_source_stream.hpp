//
// Created by bimba on 1/2/26.
//

#ifndef IAUDIOSOURCESTREAM_HPP
#define IAUDIOSOURCESTREAM_HPP
#include "IAudioSource.hpp"
#include "i_audio_stream.hpp"

class i_audio_source_stream : public IAudioSource, public i_audio_stream
{
public:
    i_audio_source_stream(const services* services, std::string id): i_audio_stream(services, std::move(id))
    {
    }

    virtual ~i_audio_source_stream() = default;
};

#endif //IAUDIOSOURCESTREAM_HPP
