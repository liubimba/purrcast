//
// Created by bimba on 11/20/25.
//

#ifndef AUDIOCHUNK_HPP
#define AUDIOCHUNK_HPP

#include <cstdint>
#include <cstddef>

typedef struct s_audio_chunk
{
    const void* data{};
    int channels{};
    size_t frames_per_buffer{};
    uint8_t bytes_per_sample{};

    [[nodiscard]] size_t bytes() const
    {
        return channels * frames_per_buffer * bytes_per_sample;
    }
} audio_chunk;

#endif //AUDIOCHUNK_HPP
