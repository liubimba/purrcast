//
// Created by bimba on 11/15/25.
//

#include "logger_factory.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

logger_factory::logger_factory():
    sink_(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>()),
    level_(spdlog::level::info)
{
}

logger_factory::~logger_factory() = default;

void logger_factory::setLevel(spdlog::level::level_enum level)
{
    level_ = level;
}

spdlog::level::level_enum logger_factory::getLevel()
{
    return level_;
}

void logger_factory::setSink(const std::shared_ptr<spdlog::sinks::sink>& sink)
{
    sink_ = sink;
}

std::shared_ptr<spdlog::sinks::sink> logger_factory::getSink()
{
    return sink_;
}

std::shared_ptr<spdlog::logger> logger_factory::create(const std::string& name)
{
    auto logger = std::make_shared<spdlog::logger>(name, sink_);
    logger->set_level(level_);
    return logger;
}

void logger_factory::drop(const std::shared_ptr<spdlog::logger>& logger)
{
    spdlog::drop(logger->name());
}

bool logger_factory::available()
{
    return true;
}
