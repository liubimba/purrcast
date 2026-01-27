//
// Created by bimba on 11/15/25.
//

#ifndef LOGGERFACTORY_HPP
#define LOGGERFACTORY_HPP
#include <memory>

#include "../services/i_service.hpp"
#include "spdlog/spdlog.h"

class logger_factory : public i_service
{
    std::shared_ptr<spdlog::sinks::sink> sink_;
    spdlog::level::level_enum level_;

public:
    logger_factory();
    ~logger_factory() override;
    void setLevel(spdlog::level::level_enum level);
    spdlog::level::level_enum getLevel();
    void setSink(const std::shared_ptr<spdlog::sinks::sink>& sink);
    std::shared_ptr<spdlog::sinks::sink> getSink();
    std::shared_ptr<spdlog::logger> create(const std::string& name);
    void drop(const std::shared_ptr<spdlog::logger>& logger);
    bool available() override;
};

#endif //LOGGERFACTORY_HPP
