//
// Created by bimba on 1/3/26.
//

#ifndef AUDIO_DEVICE_INFO_HPP
#define AUDIO_DEVICE_INFO_HPP

#include  <string>

typedef struct s_audio_device
{
    std::string name;
    std::string description;
} audio_device;

const audio_device NO_AUDIO_DEVICE = {"NOT_FOUND", "NOT_FOUND"};

#endif //AUDIO_DEVICE_INFO_HPP
