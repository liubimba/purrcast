//
// Created by bimba on 11/19/25.
//

#include "Tests.hpp"

class AudioLoopbackModule : public needs_pulse_audio
{
};

TEST_F(AudioLoopbackModule, load)
{
    settings::s_module::s_loopback params{};
    audio_loopback_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_router{}));
    ASSERT_TRUE(module.load(params));
    ASSERT_ANY_THROW(module.load(params));
}


TEST_F(AudioLoopbackModule, reload)
{
    settings::s_module::s_loopback params{};
    audio_loopback_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_router{}));
    ASSERT_TRUE(module.load(params));
    ASSERT_FALSE(module.reload(params));
    params.loopback_sink_name += TestData::uuid();
    ASSERT_TRUE(module.reload(params));
}

TEST_F(AudioLoopbackModule, unload)
{
    settings::s_module::s_loopback params{};
    audio_loopback_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.unload());
}

TEST_F(AudioLoopbackModule, loaded)
{
    settings::s_module::s_loopback params{};
    audio_loopback_module module{TestData::get_services()};
    ASSERT_FALSE(module.loaded());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.loaded());
}
