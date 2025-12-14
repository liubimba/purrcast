//
// Created by bimba on 11/19/25.
//
#include "Tests.hpp"
#include "../modules/router/AudioRouterModule.hpp"

TEST(AudioRouterModule, load)
{
    Pa_Initialize();
    settings::s_module::router params{};
    params.source.name = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->name;
    params.sink.name = "/tmp/fifo";
    AudioRouterModule module{TestData::services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::loopback()));
    ASSERT_TRUE(module.load(params));
    ASSERT_ANY_THROW(module.load(params));
}

TEST(AudioRouterModule, reload)
{
    Pa_Initialize();
    settings::s_module::router params{};
    params.source.name = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->name;
    params.sink.name = "/tmp/fifo";
    AudioRouterModule module{TestData::services()};
    ASSERT_ANY_THROW(module.reload(settings::s_module::loopback()));
    ASSERT_ANY_THROW(module.reload(params));
    ASSERT_TRUE(module.load(params));
    ASSERT_FALSE(module.reload(params));
    params.sink.name = "/tmp/fifo2";
    ASSERT_TRUE(module.reload(params));
}

TEST(AudioRouterModule, unload)
{
    Pa_Initialize();
    settings::s_module::router params{};
    params.source.name = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->name;
    params.sink.name = "/tmp/fifo";
    AudioRouterModule module{TestData::services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.unload());
}

TEST(AudioRouterModule, loaded)
{
    Pa_Initialize();
    settings::s_module::router params{};
    params.source.name = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->name;
    params.sink.name = "/tmp/fifo";
    AudioRouterModule module{TestData::services()};
    ASSERT_FALSE(module.loaded());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.loaded());
}
