//
// Created by bimba on 11/15/25.
//

#include "Tests.hpp"
#include "absl/time/clock.h"

class SnapserverModule : public needs_external_binary
{
protected:
    SnapserverModule(): needs_external_binary("MULTIROOM_SNAPSERVER_BINARY")
    {
    }

    settings::s_module::s_snapserver params()
    {
        settings::s_module::s_snapserver params;
        params.path_to_binary = binary_;
        params.config = TestData::from_environment("MULTIROOM_SNAPSERVER_CONFIG");
        return params;
    }

    static void assert_ports_bound(const settings::s_module::s_snapserver& params, bool expected)
    {
        ASSERT_EQ(os_port::is_bound(params.ports.control), expected);
        ASSERT_EQ(os_port::is_bound(params.ports.stream), expected);
        ASSERT_EQ(os_port::is_bound(params.ports.http), expected);
    }
};

TEST_F(SnapserverModule, load)
{
    settings::s_module::s_snapserver settings = params();
    snapserver_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_router{}));
    ASSERT_TRUE(module.load(settings));
    ASSERT_ANY_THROW(module.load(settings));
    absl::SleepFor(absl::Seconds(1));
    assert_ports_bound(settings, true);
}

TEST_F(SnapserverModule, unload)
{
    settings::s_module::s_snapserver settings = params();
    snapserver_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(settings));
    ASSERT_TRUE(module.unload());
    absl::SleepFor(absl::Seconds(1));
    assert_ports_bound(settings, false);
}

TEST_F(SnapserverModule, loaded)
{
    snapserver_module module{TestData::get_services()};
    ASSERT_TRUE(module.load(params()));
    ASSERT_TRUE(module.loaded());
}

TEST_F(SnapserverModule, destructor)
{
    settings::s_module::s_snapserver settings = params();
    {
        snapserver_module module{TestData::get_services()};
        ASSERT_TRUE(module.load(settings));
    }
    absl::SleepFor(absl::Seconds(1));
    assert_ports_bound(settings, false);
}
