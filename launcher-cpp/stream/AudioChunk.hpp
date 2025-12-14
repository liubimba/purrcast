//
// Created by bimba on 11/20/25.
//

#ifndef AUDIOCHUNK_HPP
#define AUDIOCHUNK_HPP

typedef struct AudioChunk
{
    const void* data{};
    size_t size{};
} AudioChunk;

#endif //AUDIOCHUNK_HPP
