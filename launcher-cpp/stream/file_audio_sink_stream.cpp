//
// Created by bimba on 12/31/25.
//

#include "file_audio_sink_stream.hpp"


file_audio_sink_stream::file_audio_sink_stream(const services* services, const std::string& id):
    i_audio_sink_stream(services, id), started_(false)

{
}

audio_stream_result file_audio_sink_stream::start(const audio_stream_params& parameters)
{
    file_.open(parameters.name.c_str());
    if (!file_.is_open())
        return audio_stream_result::failed(absl::StrFormat("Failed to open: %s", parameters.name));
    started_.store(true);
    return audio_stream_result::ok();
}

audio_stream_result file_audio_sink_stream::stop()
{
    if (file_.is_open())
        file_.close();
    started_.store(false);
    return audio_stream_result::ok();
}

bool file_audio_sink_stream::started()
{
    return started_.load();
}

void file_audio_sink_stream::receive(const audio_chunk& chunk)
{
    if (file_.is_open())
    {
        file_.write(static_cast<const char*>(chunk.data), chunk.bytes());
    }
}
