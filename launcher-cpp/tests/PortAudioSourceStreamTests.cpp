//
// Created by bimba on 11/21/25.
//

#include "Tests.hpp"

TEST(PortAudioSourceStream, start)
{
    Pa_Initialize();
    AudioStreamParameters params;
    PortAudioSourceStream stream{TestData::services(), TestData::uuid()};
    ASSERT_FALSE(stream.start(params));
    params.name = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->name;
    ASSERT_TRUE(stream.start(params));
    ASSERT_ANY_THROW(stream.start(params));
    Pa_Terminate();
}

TEST(PortAudioSourceStream, stop)
{
    Pa_Initialize();
    AudioStreamParameters params;
    params.name = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->name;
    PortAudioSourceStream stream{TestData::services(), TestData::uuid()};
    ASSERT_ANY_THROW(stream.stop());
    ASSERT_TRUE(stream.start(params));
    ASSERT_TRUE(stream.stop());
    Pa_Terminate();
}

TEST(PortAudioSourceStream, started)
{
    Pa_Initialize();
    AudioStreamParameters params;
    params.name = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->name;
    PortAudioSourceStream stream{TestData::services(), TestData::uuid()};
    ASSERT_FALSE(stream.started());
    ASSERT_TRUE(stream.start(params));
    ASSERT_TRUE(stream.started());
    Pa_Terminate();
}
