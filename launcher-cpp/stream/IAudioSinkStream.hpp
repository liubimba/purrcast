//
// Created by bimba on 1/2/26.
//

#ifndef IAUDIOSINKSTREAM_HPP
#define IAUDIOSINKSTREAM_HPP
#include "IAudioSink.hpp"
#include "IAudioStream.hpp"

class IAudioSinkStream : public IAudioSink, public IAudioStream
{
public:
    IAudioSinkStream(const Services* services, std::string id): IAudioStream(services, std::move(id))
    {
    }
};

#endif //IAUDIOSINKSTREAM_HPP
