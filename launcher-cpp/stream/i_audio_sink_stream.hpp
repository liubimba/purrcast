//
// Created by bimba on 1/2/26.
//

#ifndef IAUDIOSINKSTREAM_HPP
#define IAUDIOSINKSTREAM_HPP
#include "i_audio_sink.hpp"
#include "i_audio_stream.hpp"

class i_audio_sink_stream : public i_audio_sink, public i_audio_stream
{
public:
    i_audio_sink_stream(const services* services, std::string id): i_audio_stream(services, std::move(id))
    {
    }
};

#endif //IAUDIOSINKSTREAM_HPP
