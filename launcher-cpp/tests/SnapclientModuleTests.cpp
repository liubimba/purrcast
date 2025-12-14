//
// Created by bimba on 11/17/25.
//

#include <dirent.h>
#include <fstream>
#include <list>
#include <boost/fusion/container/list/list.hpp>

#include "Tests.hpp"
#include "../util/ProcessInfo.hpp"
#include "absl/time/clock.h"
#include "absl/time/time.h"

TEST(SnapclientModule, load)
{
    settings::s_module::snapclient params{};
    params.sinkIndex = -1;
    SnapclientModule module{TestData::services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::loopback()));
    ASSERT_FALSE(module.load(params));
    params.sinkIndex = 0;
    ASSERT_TRUE(module.load(params));
    absl::SleepFor(absl::Seconds(5));
    std::vector<ProcessInfoDescription> processes = ProcessInfo::list();
    bool foundProcess = std::any_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "snapclient";
    });
    ASSERT_TRUE(foundProcess);
}

TEST(SnapclientModule, reload)
{
    settings::s_module::snapclient params{};
    params.sinkIndex = -1;
    SnapclientModule module{TestData::services()};
    ASSERT_ANY_THROW(module.reload(params));
    ASSERT_ANY_THROW(module.reload(settings::s_module::loopback()));
    params.sinkIndex = 0;
    ASSERT_TRUE(module.load(params));
    ASSERT_FALSE(module.reload(params));
    ASSERT_ANY_THROW(module.reload(settings::s_module::loopback()));
    params.sinkIndex = 31;
    ASSERT_TRUE(module.reload(params));
    std::vector<ProcessInfoDescription> processes = ProcessInfo::list();
    bool foundProcess = std::any_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "snapclient";
    });
    ASSERT_TRUE(foundProcess);
}

TEST(SnapclientModule, unload)
{
    settings::s_module::snapclient params{};
    params.sinkIndex = 0;
    SnapclientModule module{TestData::services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.unload());
    absl::SleepFor(absl::Seconds(1));
    std::vector<ProcessInfoDescription> processes = ProcessInfo::list();
    bool notFoundProcess = std::none_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "snapclient";
    });
    ASSERT_TRUE(notFoundProcess);
}

TEST(SnapclientModule, destructor)
{
    settings::s_module::snapclient params{};
    params.sinkIndex = 0;
    {
        SnapclientModule module{TestData::services()};
        ASSERT_TRUE(module.load(params));
    }
    absl::SleepFor(absl::Seconds(1));
    std::vector<ProcessInfoDescription> processes = ProcessInfo::list();
    bool notFoundProcess = std::none_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "snapclient";
    });
    ASSERT_TRUE(notFoundProcess);
}

