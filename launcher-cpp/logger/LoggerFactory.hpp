//
// Created by bimba on 11/15/25.
//

#ifndef LOGGERFACTORY_HPP
#define LOGGERFACTORY_HPP
#include <memory>

#include "../services/IService.hpp"
#include "spdlog/spdlog.h"

class LoggerFactory : public IService
{
    std::shared_ptr<spdlog::sinks::sink> sink_;
    spdlog::level::level_enum level_;

public:
    LoggerFactory();
    ~LoggerFactory() override;
    void setLevel(spdlog::level::level_enum level);
    spdlog::level::level_enum getLevel();
    void setSink(const std::shared_ptr<spdlog::sinks::sink>& sink);
    std::shared_ptr<spdlog::sinks::sink> getSink();
    std::shared_ptr<spdlog::logger> create(const std::string& name);
    void drop(const std::shared_ptr<spdlog::logger>& logger);
    bool available() override;
};

#endif //LOGGERFACTORY_HPP
