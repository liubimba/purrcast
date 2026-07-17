//
// Created by bimba on 11/15/25.
//


#include "Tests.hpp"

namespace
{
    constexpr int UNUSED_PORT = 21734;
}

TEST(Port, reportsFreePortAsNotBound)
{
    ASSERT_FALSE(os_port::is_bound(UNUSED_PORT));
}

TEST(Port, reportsBoundPortAsBound)
{
    const std::shared_ptr<os_port> bound = os_port::bind(UNUSED_PORT);
    ASSERT_NE(bound, nullptr);
    ASSERT_TRUE(os_port::is_bound(UNUSED_PORT));
}

TEST(Port, reportsReleasedPortAsFreeAgain)
{
    const std::shared_ptr<os_port> bound = os_port::bind(UNUSED_PORT);
    ASSERT_NE(bound, nullptr);
    bound->release();
    ASSERT_FALSE(os_port::is_bound(UNUSED_PORT));
}
