//
// Created by bimba on 11/21/25.
//

#ifndef FIFOAUDIOSINKSTREAM_HPP
#define FIFOAUDIOSINKSTREAM_HPP
#include <fstream>
#include <mutex>
#include "IAudioSinkStream.hpp"
#include "../logger/LoggerFactory.hpp"


class FIFOAudioSinkStream : public IAudioSinkStream
{
public:
    explicit FIFOAudioSinkStream(const Services* services, const std::string& id);
    ~FIFOAudioSinkStream() override;
    bool start(const AudioStreamParameters& parameters) override;
    bool stop() override;
    bool started() override;
    void receive(const AudioChunk& chunk) override;

private:
    std::mutex mutex_;
    std::ofstream fifo_;
    AudioStreamParameters params_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //FIFOAUDIOSINKSTREAM_HPP
