//
// Created by bimba on 11/15/25.
//

#include "Tests.hpp"
#include "absl/time/clock.h"

TEST(SnapserverModule, load)
{
    settings::s_module::s_snapserver params;
    snapserver_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_router{}));
    ASSERT_TRUE(module.load(params));
    ASSERT_ANY_THROW(module.load(params));
    absl::SleepFor(absl::Seconds(1));
    ASSERT_TRUE(os_port::is_bound(params.ports.control));
    ASSERT_TRUE(os_port::is_bound(params.ports.stream));
    ASSERT_TRUE(os_port::is_bound(params.ports.http));
}


TEST(SnapserverModule, unload)
{
    settings::s_module::s_snapserver params;
    snapserver_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.unload());
    absl::SleepFor(absl::Seconds(1));
    ASSERT_FALSE(os_port::is_bound(params.ports.control));
    ASSERT_FALSE(os_port::is_bound(params.ports.stream));
    ASSERT_FALSE(os_port::is_bound(params.ports.http));
}

TEST(SnapserverModule, loaded)
{
    snapserver_module module{TestData::get_services()};
    ASSERT_TRUE(module.load(settings::s_module::s_snapserver()));
    ASSERT_TRUE(module.loaded());
}


TEST(SnapserverModule, destructor)
{
    settings::s_module::s_snapserver params;
    {
        snapserver_module module{TestData::get_services()};
        ASSERT_TRUE(module.load(params));
    }
    SleepFor(absl::Seconds(1));
    ASSERT_FALSE(os_port::is_bound(params.ports.control));
    ASSERT_FALSE(os_port::is_bound(params.ports.stream));
    ASSERT_FALSE(os_port::is_bound(params.ports.http));
}
