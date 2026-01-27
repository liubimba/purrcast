//
// Created by bimba on 11/21/25.
//

#ifndef FIFOAUDIOSINKSTREAM_HPP
#define FIFOAUDIOSINKSTREAM_HPP
#include <fstream>
#include <mutex>
#include "i_audio_sink.hpp"
#include "i_audio_sink_stream.hpp"
#include "i_audio_stream.hpp"
#include "../logger/logger_factory.hpp"
#include "absl/strings/str_format.h"


class fifo_audio_sink_stream : public i_audio_sink_stream
{
public:
    explicit fifo_audio_sink_stream(const services* services, const std::string& id);
    ~fifo_audio_sink_stream() override;
    audio_stream_result start(const audio_stream_params& parameters) override;
    audio_stream_result stop() override;
    bool started() override;
    void receive(const audio_chunk& chunk) override;

private:
    std::mutex mutex_;
    std::ofstream fifo_;
    audio_stream_params params_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //FIFOAUDIOSINKSTREAM_HPP
