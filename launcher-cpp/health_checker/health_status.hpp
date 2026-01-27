//
// Created by bimba on 11/22/25.
//

#ifndef HEALTHSTATUS_HPP
#define HEALTHSTATUS_HPP

#include "nlohmann/json.hpp"

enum class health_status
{
    healthy, unhealthy, not_loaded
};

NLOHMANN_JSON_SERIALIZE_ENUM(health_status, {
                             {health_status::healthy, "healthy"},
                             {health_status::unhealthy, "unhealthy"},
                             {health_status::not_loaded, "not_loaded"}
                             });

#endif //HEALTHSTATUS_HPP
