//
// Created by bimba on 11/16/25.
//

#include "Tests.hpp"

#if defined(__linux__)

#include "Tests.hpp"
#include "../util/linux_process.hpp"
TEST(LinuxProcess, ExecuteAndFinish)
{
    linux_process proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"sleep 0.1\""));
    ASSERT_EQ(proc.state(), process_state::RUNNING);

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    ASSERT_EQ(proc.state(), process_state::TERMINATED);
}

TEST(LinuxProcess, TerminateStopsProcess)
{
    linux_process proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"sleep 2\""));

    ASSERT_EQ(proc.state(), process_state::RUNNING);
    ASSERT_TRUE(proc.terminate());

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(proc.state(), process_state::TERMINATED);
}

TEST(LinuxProcess, ExecuteTwiceFails)
{
    linux_process proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"sleep 0.2\""));
    ASSERT_ANY_THROW(proc.execute("/bin/echo", "\"hi\""));
    ASSERT_EQ(proc.state(), process_state::RUNNING);
}

TEST(LinuxProcess, ThreadSafetyRunning)
{
    linux_process proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"sleep 0.3\""));

    std::atomic<bool> go{true};
    std::vector<std::thread> threads;

    for (int i = 0; i < 32; i++)
    {
        threads.emplace_back([&]()
        {
            while (go.load())
            {
                proc.state();
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    proc.terminate();
    go = false;

    for (auto& t : threads) t.join();
    ASSERT_EQ(proc.state(), process_state::TERMINATED);
}

TEST(LinuxProcess, ManyTerminateCallsNoDeadlock)
{
    linux_process proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"sleep 1\""));

    std::vector<std::thread> threads;

    for (int i = 0; i < 16; i++)
    {
        threads.emplace_back([&]()
        {
            for (int j = 0; j < 100; j++)
                try
                {
                    proc.terminate();
                }
                catch (const std::exception& e)
                {
                    ASSERT_TRUE(absl::StartsWith(e.what(), "LinuxProcess"));
                }
        });
    }

    for (auto& t : threads) t.join();
    ASSERT_EQ(proc.state(), process_state::TERMINATED);
}

TEST(LinuxProcess, DestructorDoesNotDeadlock)
{
    {
        linux_process proc{TestData::services(), TestData::uuid()};
        proc.execute("/bin/sh", "-c \"sleep 0.1\"");
    }

    // если есть дедлок, тест просто зависнет
    SUCCEED();
}

struct DummyListener : i_process_listener
{
    void onTerminate() override
    {
    }
};

TEST(LinuxProcess, AddListenerIsThreadSafe)
{
    linux_process proc{TestData::services(), TestData::uuid()};
    DummyListener listener;

    std::vector<std::thread> threads;

    for (int i = 0; i < 16; i++)
    {
        threads.emplace_back([&]()
        {
            for (int j = 0; j < 100; j++)
                proc.add_listener(&listener);
        });
    }

    for (auto& t : threads) t.join();
    SUCCEED();
}

TEST(LinuxProcess, MonitorDetectsExit)
{
    linux_process proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"echo hi\""));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ASSERT_EQ(proc.state(), process_state::TERMINATED);
}

#endif
