//
// Created by bimba on 12/31/25.
//

#ifndef FILEAUDIOSINKSTREAM_HPP
#define FILEAUDIOSINKSTREAM_HPP
#include "i_audio_sink.hpp"
#include <fstream>
#include "absl/strings/str_format.h"

#include "i_audio_sink_stream.hpp"
#include "i_audio_stream.hpp"

class file_audio_sink_stream : public i_audio_sink_stream
{
public:
    file_audio_sink_stream(const services* services, const std::string& id);

    audio_stream_result start(const audio_stream_params& parameters) override;
    audio_stream_result stop() override;
    bool started() override;
    void receive(const audio_chunk& chunk) override;

private:
    std::ofstream file_;
    std::atomic<bool> started_;
};


#endif //FILEAUDIOSINKSTREAM_HPP
