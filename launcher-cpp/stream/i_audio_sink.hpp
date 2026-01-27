//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOSINK_HPP
#define IAUDIOSINK_HPP
#include "s_audio_chunk.hpp"

class i_audio_sink
{
public:
    virtual ~i_audio_sink() = default;

    virtual void receive(const audio_chunk& chunk) = 0;
};

#endif //IAUDIOSINKSTREAM_HPP
