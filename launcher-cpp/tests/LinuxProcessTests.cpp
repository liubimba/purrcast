//
// Created by bimba on 11/16/25.
//

#include "Tests.hpp"

#if defined(__linux__)

#include "Tests.hpp"
#include "../util/LinuxProcess.hpp"
TEST(LinuxProcess, ExecuteAndFinish)
{
    LinuxProcess proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"sleep 0.1\""));
    ASSERT_EQ(proc.state(), ProcessState::RUNNING);

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    ASSERT_EQ(proc.state(), ProcessState::TERMINATED);
}

TEST(LinuxProcess, TerminateStopsProcess)
{
    LinuxProcess proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"sleep 2\""));

    ASSERT_EQ(proc.state(), ProcessState::RUNNING);
    ASSERT_TRUE(proc.terminate());

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(proc.state(), ProcessState::TERMINATED);
}

TEST(LinuxProcess, ExecuteTwiceFails)
{
    LinuxProcess proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"sleep 0.2\""));
    ASSERT_ANY_THROW(proc.execute("/bin/echo", "\"hi\""));
    ASSERT_EQ(proc.state(), ProcessState::RUNNING);
}

TEST(LinuxProcess, ThreadSafetyRunning)
{
    LinuxProcess proc{TestData::services(), TestData::uuid()};
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
    ASSERT_EQ(proc.state(), ProcessState::TERMINATED);
}

TEST(LinuxProcess, ManyTerminateCallsNoDeadlock)
{
    LinuxProcess proc{TestData::services(), TestData::uuid()};
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
    ASSERT_EQ(proc.state(), ProcessState::TERMINATED);
}

TEST(LinuxProcess, DestructorDoesNotDeadlock)
{
    {
        LinuxProcess proc{TestData::services(), TestData::uuid()};
        proc.execute("/bin/sh", "-c \"sleep 0.1\"");
    }

    // если есть дедлок, тест просто зависнет
    SUCCEED();
}

struct DummyListener : IProcessListener
{
    void onTerminate() override
    {
    }
};

TEST(LinuxProcess, AddListenerIsThreadSafe)
{
    LinuxProcess proc{TestData::services(), TestData::uuid()};
    DummyListener listener;

    std::vector<std::thread> threads;

    for (int i = 0; i < 16; i++)
    {
        threads.emplace_back([&]()
        {
            for (int j = 0; j < 100; j++)
                proc.addListener(&listener);
        });
    }

    for (auto& t : threads) t.join();
    SUCCEED();
}

TEST(LinuxProcess, MonitorDetectsExit)
{
    LinuxProcess proc{TestData::services(), TestData::uuid()};
    ASSERT_TRUE(proc.execute("/bin/sh", "-c \"echo hi\""));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ASSERT_EQ(proc.state(), ProcessState::TERMINATED);
}

#endif
