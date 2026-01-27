//
// Created by bimba on 1/15/26.
//

#ifndef PROXYAUDIOSINKLISTENER_HPP
#define PROXYAUDIOSINKLISTENER_HPP
#include <functional>

#include "../../../stream/i_audio_sink.hpp"


class proxy_audio_sink_listener : public i_audio_sink
{
    std::function<void(const audio_chunk&)> consumer_;

public:
    proxy_audio_sink_listener(std::function<void(const audio_chunk&)> consumer);
    void receive(const audio_chunk& chunk) override;
};


#endif //PROXYAUDIOSINKLISTENER_HPP
