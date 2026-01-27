//
// Created by bimba on 1/15/26.
//

#ifndef IAUDIOINSPECTOR_HPP
#define IAUDIOINSPECTOR_HPP
#include "../../../stream/s_audio_chunk.hpp"
#include "../data/verification_report.hpp"
#include "../data/verification_stats.hpp"

class i_audio_inspector
{
public:
    virtual ~i_audio_inspector() = default;
    virtual void add_reference(const audio_chunk& chunk, uint64_t seq) = 0;
    virtual verification_report verify(const audio_chunk& chunk) = 0;
    [[nodiscard]] virtual const verification_stats& get_stats() const = 0;
    virtual void reset_stats() = 0;
};

#endif //IAUDIOINSPECTOR_HPP
