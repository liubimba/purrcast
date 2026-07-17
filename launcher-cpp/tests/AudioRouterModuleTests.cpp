//
// Created by bimba on 11/19/25.
//
#include "Tests.hpp"
#include "../modules/router/audio_router_module.hpp"

class AudioRouterModule : public needs_audio_input_device
{
protected:
    settings::s_module::s_router params(const std::string& sink = "/tmp/fifo")
    {
        settings::s_module::s_router params{};
        params.source.name = device_name_;
        params.sink.name = sink;
        return params;
    }
};

TEST_F(AudioRouterModule, load)
{
    audio_router_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_loopback{}));
    ASSERT_TRUE(module.load(params()));
    ASSERT_ANY_THROW(module.load(params()));
}

TEST_F(AudioRouterModule, reload)
{
    audio_router_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.reload(settings::s_module::s_loopback{}));
    ASSERT_ANY_THROW(module.reload(params()));
    ASSERT_TRUE(module.load(params()));
    ASSERT_FALSE(module.reload(params()));
    ASSERT_TRUE(module.reload(params("/tmp/fifo2")));
}

TEST_F(AudioRouterModule, unload)
{
    audio_router_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params()));
    ASSERT_TRUE(module.unload());
}

TEST_F(AudioRouterModule, loaded)
{
    audio_router_module module{TestData::get_services()};
    ASSERT_FALSE(module.loaded());
    ASSERT_TRUE(module.load(params()));
    ASSERT_TRUE(module.loaded());
}
