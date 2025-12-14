//
// Created by bimba on 11/15/25.
//

#include "Tests.hpp"

TEST(LoggerFactory, create)
{
    LoggerFactory factory;
    auto logger = factory.create("test");
    ASSERT_NE(logger, nullptr);
    ASSERT_NE(logger.get(), nullptr);
}

TEST(LoggerFactory, level)
{
    LoggerFactory factory;
    spdlog::level::level_enum level = spdlog::level::trace;
    factory.setLevel(level);
    ASSERT_EQ(level, factory.getLevel());
    auto logger = factory.create("test");
    ASSERT_EQ(logger->level(), level);
}

TEST(LoggerFactory, sink)
{
    auto sink = std::make_shared<spdlog::sinks::daily_file_sink_st>("log.txt", 0, 0);
    LoggerFactory factory;
    factory.setSink(sink);
    ASSERT_EQ(sink, factory.getSink());
    auto logger = factory.create("test");

    auto sinks = logger->sinks();
    bool foundSink = std::any_of(sinks.begin(), sinks.end(), [&](const auto& loggerSink)
    {
        return sink == loggerSink;
    });
    ASSERT_TRUE(foundSink);
}
