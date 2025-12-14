//
// Created by bimba on 11/16/25.
//


#include "Tests.hpp"
#include "../util/Executor.hpp"

TEST(Executor, post)
{
    Executor executor{TestData::services(), TestData::uuid()};
    std::future<void> future = executor.post<void>([=]()
    {
    });
    ASSERT_TRUE(future.wait_for(TestData::timeout()) == std::future_status::ready);
    executor.join();
}

TEST(Executor, run_return_task)
{
    Executor executor{TestData::services(), TestData::uuid()};
    auto f = executor.post<int>([]() { return 123; });
    ASSERT_EQ(f.get(), 123);
    executor.join();
}

TEST(Executor, many_tasks)
{
    Executor executor{TestData::services(), TestData::uuid()};

    auto f1 = executor.post<int>([]() { return 1; });
    auto f2 = executor.post<int>([]() { return 2; });
    auto f3 = executor.post<int>([]() { return 3; });

    ASSERT_EQ(f1.get(), 1);
    ASSERT_EQ(f2.get(), 2);
    ASSERT_EQ(f3.get(), 3);

    executor.join();
}

TEST(Executor, exception_propagates)
{
    Executor executor{TestData::services(), TestData::uuid()};

    auto f = executor.post<void>([]() { throw std::runtime_error("boom"); });

    ASSERT_THROW(f.get(), std::runtime_error);

    executor.join();
}

TEST(Executor, join_stops_loop)
{
    Executor executor{TestData::services(), TestData::uuid()};
    executor.join();
    ASSERT_THROW(executor.post<void>([]{}), std::runtime_error);
}

TEST(Executor, destructor_joins)
{
    {
        Executor executor{TestData::services(), TestData::uuid()};
        executor.post<void>([]()
        {
        });
    }
    SUCCEED();
}

TEST(Executor, multithread_post)
{
    Executor executor{TestData::services(), TestData::uuid()};
    constexpr int THREAD_COUNT = 100;
    constexpr int TASKS_PER_THREAD = 50;
    std::vector<std::thread> threads;
    std::vector<std::future<int>> futures;
    futures.reserve(THREAD_COUNT * TASKS_PER_THREAD);
    std::mutex futMux;
    for (int t = 0; t < THREAD_COUNT; t++)
    {
        threads.emplace_back([&]()
        {
            for (int i = 0; i < TASKS_PER_THREAD; i++)
            {
                auto f = executor.post<int>([]() { return 1; });
                std::lock_guard<std::mutex> lk(futMux);
                futures.push_back(std::move(f));
            }
        });
    }
    for (auto& th : threads)
        th.join();

    for (auto& f : futures)
        ASSERT_EQ(f.get(), 1);
    executor.join();
}


