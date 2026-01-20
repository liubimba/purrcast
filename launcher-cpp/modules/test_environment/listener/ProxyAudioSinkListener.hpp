//
// Created by bimba on 1/15/26.
//

#ifndef PROXYAUDIOSINKLISTENER_HPP
#define PROXYAUDIOSINKLISTENER_HPP
#include <functional>

#include "../../../stream/IAudioSink.hpp"


class ProxyAudioSinkListener : public IAudioSink
{
    std::function<void(const audio_chunk&)> consumer_;

public:
    ProxyAudioSinkListener(std::function<void(const audio_chunk&)> consumer);
    void receive(const audio_chunk& chunk) override;
};


#endif //PROXYAUDIOSINKLISTENER_HPP
