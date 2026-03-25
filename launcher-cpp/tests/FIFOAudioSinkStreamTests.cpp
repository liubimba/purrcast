//
// Created by bimba on 11/21/25.
//

#include "Tests.hpp"

TEST(FIFOAudioSinkStream, start)
{
    audio_stream_params parameters;
    fifo_audio_sink_stream stream{TestData::get_services(), TestData::uuid()};
    ASSERT_FALSE(stream.start(parameters).is_ok);
    parameters.name = "/tmp/fifo";
    ASSERT_TRUE(stream.start(parameters).is_ok);
}

TEST(FIFOAudioSinkStream, stop)
{
    audio_stream_params parameters;
    fifo_audio_sink_stream stream{TestData::get_services(), TestData::uuid()};
    ASSERT_ANY_THROW(stream.stop());
    parameters.name = "/tmp/fifo";
    ASSERT_TRUE(stream.start(parameters).is_ok);
    ASSERT_TRUE(stream.stop().is_ok);
}

TEST(FIFOAudioSinkStream, started)
{
    audio_stream_params parameters;
    fifo_audio_sink_stream stream{TestData::get_services(), TestData::uuid()};
    ASSERT_FALSE(stream.started());
    parameters.name = "/tmp/fifo";
    ASSERT_TRUE(stream.start(parameters).is_ok);
    ASSERT_TRUE(stream.started());
    ASSERT_TRUE(stream.stop().is_ok);
    ASSERT_FALSE(stream.started());
}


