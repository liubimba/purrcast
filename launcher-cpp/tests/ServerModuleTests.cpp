//
// Created by bimba on 12/8/25.
//

#include "Tests.hpp"
#include <filesystem>
#include "../util/process_info.hpp"
#include "absl/time/clock.h"

class ServerModule : public needs_external_binary
{
protected:
    ServerModule(): needs_external_binary("MULTIROOM_SERVER_BINARY")
    {
    }

    void SetUp() override
    {
        needs_external_binary::SetUp();
        if (IsSkipped())
            return;
        port_ = free_port_pair();
    }

    settings::s_module::s_server params(int port)
    {
        settings::s_module::s_server params{};
        params.bin = binary_;
        params.port = port;
        params.static_dir = std::filesystem::temp_directory_path().string();
        return params;
    }

    settings::s_module::s_server params()
    {
        return params(port_);
    }

    static bool server_process_running()
    {
        std::vector<ProcessInfoDescription> processes = process_info::list();
        return std::any_of(processes.begin(), processes.end(), [](const ProcessInfoDescription& desc)
        {
            return desc.cmd == "server";
        });
    }

    static void await_port_released(int port)
    {
        for (int attempt = 0; attempt < 50 && os_port::is_bound(port); attempt++)
            absl::SleepFor(absl::Milliseconds(100));
    }

    int port_ = 0;

private:
    static int free_port_pair()
    {
        for (int candidate = 3456; candidate < 4000; candidate++)
            if (!os_port::is_bound(candidate) && !os_port::is_bound(candidate + 1))
                return candidate;
        throw std::runtime_error("no free port pair for the server tests");
    }
};

TEST(ServerModuleWithoutBinary, refusesParametersForAnotherModule)
{
    server_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.load(settings::s_module::s_loopback()));
}

TEST_F(ServerModule, refusesToLoadOnAnInvalidPort)
{
    settings::s_module::s_server invalid = params(-1);
    server_module module{TestData::get_services()};
    ASSERT_FALSE(module.load(invalid));
}

TEST_F(ServerModule, load)
{
    server_module module{TestData::get_services()};
    ASSERT_TRUE(module.load(params()));
    absl::SleepFor(absl::Seconds(5));
    ASSERT_TRUE(server_process_running());
    ASSERT_TRUE(os_port::is_bound(port_));
}

TEST_F(ServerModule, reload)
{
    server_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.reload(params()));
    ASSERT_ANY_THROW(module.reload(settings::s_module::s_loopback()));
    ASSERT_TRUE(module.load(params()));
    ASSERT_FALSE(module.reload(params()));
    ASSERT_ANY_THROW(module.reload(settings::s_module::s_loopback()));
    ASSERT_TRUE(module.reload(params(port_ + 1)));
    absl::SleepFor(absl::Seconds(5));
    ASSERT_TRUE(server_process_running());
    ASSERT_TRUE(os_port::is_bound(port_ + 1));
}

TEST_F(ServerModule, unload)
{
    server_module module{TestData::get_services()};
    ASSERT_ANY_THROW(module.unload());
    ASSERT_TRUE(module.load(params()));
    ASSERT_TRUE(module.unload());
    await_port_released(port_);
    ASSERT_FALSE(server_process_running());
    ASSERT_FALSE(os_port::is_bound(port_));
}

TEST_F(ServerModule, destructor)
{
    {
        server_module module{TestData::get_services()};
        ASSERT_TRUE(module.load(params()));
    }
    await_port_released(port_);
    ASSERT_FALSE(server_process_running());
    ASSERT_FALSE(os_port::is_bound(port_));
}
