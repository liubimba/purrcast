//
// Created by bimba on 12/23/25.
//

#ifndef MODULESTATUS_HPP
#define MODULESTATUS_HPP
#include <cstdint>
#include <string>

#include "ModuleDescription.hpp"
#include "../settings/Settings.hpp"

enum class Severity
{
    MINOR, NORMAL, IMPORTANT, CRITICAL, BLOCK
};

enum StatusCode
{
    OK = 0
};

typedef struct s_Status
{
    StatusCode code;
    Severity severity;
    std::string advice;
    std::string message;
    uint64_t timestampNs;
    ModuleParams params;
} ModuleStatus;

#endif //MODULESTATUS_HPP
