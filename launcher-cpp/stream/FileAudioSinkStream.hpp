//
// Created by bimba on 12/31/25.
//

#ifndef FILEAUDIOSINKSTREAM_HPP
#define FILEAUDIOSINKSTREAM_HPP
#include "IAudioSink.hpp"
#include <fstream>

#include "IAudioSinkStream.hpp"
#include "IAudioStream.hpp"

class FileAudioSinkStream : public IAudioSinkStream
{
public:
    FileAudioSinkStream(const Services* services, const std::string& id);

    bool start(const AudioStreamParameters& parameters) override;
    bool stop() override;
    bool started() override;
    void receive(const audio_chunk& chunk) override;

private:
    std::ofstream file_;
    std::atomic<bool> started_;
};


#endif //FILEAUDIOSINKSTREAM_HPP
