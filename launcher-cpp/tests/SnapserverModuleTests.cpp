//
// Created by bimba on 11/15/25.
//

#include "Tests.hpp"
#include "absl/time/clock.h"

TEST(SnapserverModule, load)
{
    settings::s_module::snapserver params;
    SnapserverModule module{TestData::services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::router()));
    ASSERT_TRUE(module.load(params));
    ASSERT_ANY_THROW(module.load(params));
    absl::SleepFor(absl::Seconds(1));
    ASSERT_TRUE(Port::isBound(params.ports.control));
    ASSERT_TRUE(Port::isBound(params.ports.stream));
    ASSERT_TRUE(Port::isBound(params.ports.http));
}

TEST(SnapserverModule, reload)
{
    settings::s_module::snapserver params;
    params.cmd = "echo";
    SnapserverModule module{TestData::services()};
    ASSERT_ANY_THROW(module.reload(settings::s_module::router()));
    ASSERT_ANY_THROW(module.reload(settings::s_module::snapserver()));
    ASSERT_TRUE(module.load(settings::s_module::snapserver()));
    ASSERT_TRUE(module.reload(params));
}

TEST(SnapserverModule, unload)
{
    settings::s_module::snapserver params;
    SnapserverModule module{TestData::services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.unload());
    absl::SleepFor(absl::Seconds(1));
    ASSERT_FALSE(Port::isBound(params.ports.control));
    ASSERT_FALSE(Port::isBound(params.ports.stream));
    ASSERT_FALSE(Port::isBound(params.ports.http));
}

TEST(SnapserverModule, loaded)
{
    SnapserverModule module{TestData::services()};
    ASSERT_TRUE(module.load(settings::s_module::snapserver()));
    ASSERT_TRUE(module.loaded());
}


TEST(SnapserverModule, destructor)
{
    settings::s_module::snapserver params;
    {
        SnapserverModule module{TestData::services()};
        ASSERT_TRUE(module.load(params));
    }
    SleepFor(absl::Seconds(1));
    ASSERT_FALSE(Port::isBound(params.ports.control));
    ASSERT_FALSE(Port::isBound(params.ports.stream));
    ASSERT_FALSE(Port::isBound(params.ports.http));
}
