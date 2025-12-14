//
// Created by bimba on 12/8/25.
//

#include "Tests.hpp"
#include "../util/ProcessInfo.hpp"
#include "absl/time/clock.h"

static int port = 3456;

TEST(ServerModule, load)
{
    settings::s_module::s_server params{};
    params.port = -1;
    ServerModule module{TestData::services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_loopback()));
    ASSERT_FALSE(module.load(params));
    params.port = port;
    ASSERT_TRUE(module.load(params));
    absl::SleepFor(absl::Seconds(5));
    std::vector<ProcessInfoDescription> processes = ProcessInfo::list();
    bool foundProcess = std::any_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "server";
    });
    ASSERT_TRUE(foundProcess);
    ASSERT_TRUE(Port::isBound(port));
    port++;
}

TEST(ServerModule, reload)
{
    settings::s_module::s_server params{};
    params.port = port;
    ServerModule module{TestData::services()};
    ASSERT_ANY_THROW(module.reload(params));
    ASSERT_ANY_THROW(module.reload(settings::s_module::s_loopback()));
    ASSERT_TRUE(module.load(params));
    ASSERT_FALSE(module.reload(params));
    ASSERT_ANY_THROW(module.reload(settings::s_module::s_loopback()));
    params.port = port + 1;
    ASSERT_TRUE(module.reload(params));
    absl::SleepFor(absl::Seconds(5));
    std::vector<ProcessInfoDescription> processes = ProcessInfo::list();
    bool foundProcess = std::any_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "server";
    });
    ASSERT_TRUE(foundProcess);
    ASSERT_TRUE(Port::isBound(port + 1));
    port++;
}

TEST(ServerModule, unload)
{
    settings::s_module::s_server params{};
    params.port = port;
    ServerModule module{TestData::services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params));
    ASSERT_TRUE(module.unload());
    absl::SleepFor(absl::Seconds(5));
    std::vector<ProcessInfoDescription> processes = ProcessInfo::list();
    bool notFoundProcess = std::none_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "server";
    });
    ASSERT_TRUE(notFoundProcess);
    ASSERT_FALSE(Port::isBound(port));
    port++;
}

TEST(ServerModule, destructor)
{
    settings::s_module::s_server params{};
    params.port = port;
    {
        ServerModule module{TestData::services()};
        ASSERT_TRUE(module.load(params));
    }
    absl::SleepFor(absl::Seconds(5));
    std::vector<ProcessInfoDescription> processes = ProcessInfo::list();
    bool notFoundProcess = std::none_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
    {
        return desc.cmd == "server";
    });
    ASSERT_TRUE(notFoundProcess);
    ASSERT_FALSE(Port::isBound(port));
    port++;
}
