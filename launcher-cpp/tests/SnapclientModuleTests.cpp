//
// Created by bimba on 11/17/25.
//

#include <dirent.h>
#include <fstream>
#include <list>
#include <boost/fusion/container/list/list.hpp>

#include "Tests.hpp"
#include "../util/process_info.hpp"
#include "absl/time/clock.h"
#include "absl/time/time.h"

TEST(SnapclientModule, load)
{
    settings::s_module::s_snapclient params{};
    params.soundcard = "-1";
    snapclient_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_loopback{}));
    ASSERT_FALSE(module.load(params));
    params.soundcard = "0";
    ASSERT_TRUE(module.load(params));
    absl::SleepFor(absl::Seconds(5));
    std::vector<ProcessInfoDescription> processes = process_info::list();
    bool foundProcess = std::any_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "snapclient";
    });
    ASSERT_TRUE(foundProcess);
}


TEST(SnapclientModule, unload)
{
    settings::s_module::s_snapclient params{};
    params.soundcard = "0";
    snapclient_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.unload());
    absl::SleepFor(absl::Seconds(1));
    std::vector<ProcessInfoDescription> processes = process_info::list();
    bool notFoundProcess = std::none_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "snapclient";
    });
    ASSERT_TRUE(notFoundProcess);
}

TEST(SnapclientModule, destructor)
{
    settings::s_module::s_snapclient params{};
    params.soundcard = "0";
    {
        snapclient_module module{TestData::get_services()};
        ASSERT_TRUE(module.load(params));
    }
    absl::SleepFor(absl::Seconds(1));
    std::vector<ProcessInfoDescription> processes = process_info::list();
    bool notFoundProcess = std::none_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "snapclient";
    });
    ASSERT_TRUE(notFoundProcess);
}

