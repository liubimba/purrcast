//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOSINK_HPP
#define IAUDIOSINK_HPP
#include "s_audio_chunk.hpp"

class IAudioSink
{
public:
    virtual ~IAudioSink() = default;

    virtual void receive(const audio_chunk& chunk) = 0;
};

#endif //IAUDIOSINKSTREAM_HPP
