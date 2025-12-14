//
// Created by bimba on 11/15/25.
//

#include "LoggerFactory.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

LoggerFactory::LoggerFactory():
    sink_(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>()),
    level_(spdlog::level::info)
{
}

LoggerFactory::~LoggerFactory() = default;

void LoggerFactory::setLevel(spdlog::level::level_enum level)
{
    level_ = level;
}

spdlog::level::level_enum LoggerFactory::getLevel()
{
    return level_;
}

void LoggerFactory::setSink(const std::shared_ptr<spdlog::sinks::sink>& sink)
{
    sink_ = sink;
}

std::shared_ptr<spdlog::sinks::sink> LoggerFactory::getSink()
{
    return sink_;
}

std::shared_ptr<spdlog::logger> LoggerFactory::create(const std::string& name)
{
    auto logger = std::make_shared<spdlog::logger>(name, sink_);
    logger->set_level(level_);
    return logger;
}

void LoggerFactory::drop(const std::shared_ptr<spdlog::logger>& logger)
{
    spdlog::drop(logger->name());
}

bool LoggerFactory::available()
{
    return true;
}
