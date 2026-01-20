//
// Created by bimba on 1/15/26.
//

#ifndef IAUDIOINSPECTOR_HPP
#define IAUDIOINSPECTOR_HPP
#include "../../../stream/s_audio_chunk.hpp"
#include "../data/VerificationReport.hpp"
#include "../data/VerificationStats.hpp"

class IAudioInspector
{
public:
    virtual ~IAudioInspector() = default;
    virtual void add_reference(const audio_chunk& chunk, uint64_t seq) = 0;
    virtual VerificationReport verify(const audio_chunk& chunk) = 0;
    [[nodiscard]] virtual const Stats& get_stats() const = 0;
    virtual void reset_stats() = 0;
};

#endif //IAUDIOINSPECTOR_HPP
