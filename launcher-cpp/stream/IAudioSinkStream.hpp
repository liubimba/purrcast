//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOSINKSTREAM_HPP
#define IAUDIOSINKSTREAM_HPP
#include "AudioChunk.hpp"
#include "IAudioStream.hpp"

class IAudioSinkStream : public IAudioStream
{
public:
    explicit IAudioSinkStream(const Services* services, std::string id):
        IAudioStream(services, std::move(id))
    {
    }

    ~IAudioSinkStream() override = default;

    virtual void receive(const AudioChunk& chunk) = 0;
};

#endif //IAUDIOSINKSTREAM_HPP
