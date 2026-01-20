//
// Created by bimba on 1/2/26.
//

#ifndef IAUDIOSOURCESTREAM_HPP
#define IAUDIOSOURCESTREAM_HPP
#include "IAudioSource.hpp"
#include "IAudioStream.hpp"

class IAudioSourceStream : public IAudioSource, public IAudioStream
{
public:
    IAudioSourceStream(const Services* services, std::string id): IAudioStream(services, std::move(id))
    {
    }

    virtual ~IAudioSourceStream() = default;
};

#endif //IAUDIOSOURCESTREAM_HPP
