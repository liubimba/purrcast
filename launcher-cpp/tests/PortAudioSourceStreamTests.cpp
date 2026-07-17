//
// Created by bimba on 11/21/25.
//

#include "Tests.hpp"

class PortAudioSourceStream : public needs_audio_input_device
{
protected:
    audio_stream_params params()
    {
        audio_stream_params params;
        params.name = device_name_;
        return params;
    }
};

TEST_F(PortAudioSourceStream, start)
{
    port_audio_source_stream stream{TestData::get_services(), TestData::uuid()};
    ASSERT_FALSE(stream.start(audio_stream_params{}).is_ok);
    ASSERT_TRUE(stream.start(params()).is_ok);
    ASSERT_ANY_THROW(stream.start(params()));
}

TEST_F(PortAudioSourceStream, stop)
{
    port_audio_source_stream stream{TestData::get_services(), TestData::uuid()};
    ASSERT_ANY_THROW(stream.stop());
    ASSERT_TRUE(stream.start(params()).is_ok);
    ASSERT_TRUE(stream.stop().is_ok);
}

TEST_F(PortAudioSourceStream, started)
{
    port_audio_source_stream stream{TestData::get_services(), TestData::uuid()};
    ASSERT_FALSE(stream.started());
    ASSERT_TRUE(stream.start(params()).is_ok);
    ASSERT_TRUE(stream.started());
}
