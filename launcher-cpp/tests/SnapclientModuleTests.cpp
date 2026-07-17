//
// Created by bimba on 11/17/25.
//

#include "Tests.hpp"
#include "../util/process_info.hpp"
#include "absl/time/clock.h"
#include "absl/time/time.h"

class SnapclientModule : public needs_external_binary
{
protected:
    SnapclientModule(): needs_external_binary("PURRCAST_SNAPCLIENT_BINARY")
    {
    }

    settings::s_module::s_snapclient params()
    {
        settings::s_module::s_snapclient params{};
        params.path_to_binary = binary_;
        params.soundcard = "0";
        return params;
    }

    static bool snapclient_process_running()
    {
        std::vector<ProcessInfoDescription> processes = process_info::list();
        return std::any_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
        {
            return desc.cmd == "snapclient";
        });
    }
};

TEST(SnapclientModuleWithoutBinary, refusesParametersForAnotherModule)
{
    snapclient_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_loopback{}));
}

TEST(SnapclientModuleWithoutBinary, refusesToLoadWhenBinaryDoesNotExist)
{
    settings::s_module::s_snapclient params{};
    params.path_to_binary = "/nonexistent/snapclient";
    params.soundcard = "0";
    snapclient_module module{TestData::get_services()};
    ASSERT_FALSE(module.load(params));
}

TEST_F(SnapclientModule, load)
{
    snapclient_module module{TestData::get_services()};
    ASSERT_TRUE(module.load(params()));
    absl::SleepFor(absl::Seconds(5));
    ASSERT_TRUE(snapclient_process_running());
}

TEST_F(SnapclientModule, unload)
{
    snapclient_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params()));
    ASSERT_TRUE(module.unload());
    absl::SleepFor(absl::Seconds(1));
    ASSERT_FALSE(snapclient_process_running());
}

TEST_F(SnapclientModule, destructor)
{
    {
        snapclient_module module{TestData::get_services()};
        ASSERT_TRUE(module.load(params()));
    }
    absl::SleepFor(absl::Seconds(1));
    ASSERT_FALSE(snapclient_process_running());
}
