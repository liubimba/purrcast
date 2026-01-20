//
// Created by bimba on 1/15/26.
//

#include "ProxyAudioSinkListener.hpp"

ProxyAudioSinkListener::ProxyAudioSinkListener(std::function<void(const audio_chunk&)> consumer):
    consumer_(std::move(consumer))
{
}

void ProxyAudioSinkListener::receive(const audio_chunk& chunk)
{
    consumer_(chunk);
}
