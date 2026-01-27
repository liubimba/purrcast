//
// Created by bimba on 1/24/26.
//

#ifndef AUDIO_STREAM_RESULT_HPP
#define AUDIO_STREAM_RESULT_HPP
#include <string>

typedef struct s_audio_stream_result
{
    bool is_ok;
    std::string message;

    static s_audio_stream_result ok()
    {
        return ok("");
    }

    static s_audio_stream_result ok(const std::string& message)
    {
        return s_audio_stream_result{.is_ok = true, .message = message};
    }

    static s_audio_stream_result failed(const std::string& error_message)
    {
        return s_audio_stream_result{.is_ok = false, .message = error_message};
    }
} audio_stream_result;

#endif //AUDIO_STREAM_RESULT_HPP
