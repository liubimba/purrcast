//
// Created by bimba on 1/15/26.
//

#include "proxy_audio_sink_listener.hpp"

proxy_audio_sink_listener::proxy_audio_sink_listener(std::function<void(const audio_chunk&)> consumer):
    consumer_(std::move(consumer))
{
}

void proxy_audio_sink_listener::receive(const audio_chunk& chunk)
{
    consumer_(chunk);
}
