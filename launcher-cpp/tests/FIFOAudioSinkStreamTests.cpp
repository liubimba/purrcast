//
// Created by bimba on 11/21/25.
//

#include "Tests.hpp"

TEST(FIFOAudioSinkStream, start)
{
    AudioStreamParameters parameters;
    FIFOAudioSinkStream stream{TestData::services(), TestData::uuid()};
    ASSERT_FALSE(stream.start(parameters));
    parameters.name = "/tmp/fifo";
    ASSERT_TRUE(stream.start(parameters));
}

TEST(FIFOAudioSinkStream, stop)
{
    AudioStreamParameters parameters;
    FIFOAudioSinkStream stream{TestData::services(), TestData::uuid()};
    ASSERT_ANY_THROW(stream.stop());
    parameters.name = "/tmp/fifo";
    ASSERT_TRUE(stream.start(parameters));
    ASSERT_TRUE(stream.stop());
}

TEST(FIFOAudioSinkStream, started)
{
    AudioStreamParameters parameters;
    FIFOAudioSinkStream stream{TestData::services(), TestData::uuid()};
    ASSERT_FALSE(stream.started());
    parameters.name = "/tmp/fifo";
    ASSERT_TRUE(stream.start(parameters));
    ASSERT_TRUE(stream.started());
    ASSERT_TRUE(stream.stop());
    ASSERT_FALSE(stream.started());
}


