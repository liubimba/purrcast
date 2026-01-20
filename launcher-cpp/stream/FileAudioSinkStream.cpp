//
// Created by bimba on 12/31/25.
//

#include "FileAudioSinkStream.hpp"

FileAudioSinkStream::FileAudioSinkStream(const Services* services, const std::string& id):
    IAudioSinkStream(services, id), started_(false)

{
}

bool FileAudioSinkStream::start(const AudioStreamParameters& parameters)
{
    file_.open(parameters.name.c_str());
    if (!file_.is_open())
        return false;
    started_.store(true);
    return true;
}

bool FileAudioSinkStream::stop()
{
    if (file_.is_open())
    {
        file_.close();
    }
    started_.store(false);
    return true;
}

bool FileAudioSinkStream::started()
{
    return started_.load();
}

void FileAudioSinkStream::receive(const audio_chunk& chunk)
{
    if (file_.is_open())
    {
        file_.write(static_cast<const char*>(chunk.data), chunk.bytes());
    }
}
