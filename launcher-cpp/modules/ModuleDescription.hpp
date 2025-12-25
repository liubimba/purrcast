//
// Created by bimba on 11/22/25.
//

#ifndef MODULEDESCRIPTION_HPP
#define MODULEDESCRIPTION_HPP
#include <string>
#include <vector>

#include "../libs/snapcast/common/json.hpp"

struct module_description
{
    virtual ~module_description() = default;
    std::string name;
    std::vector<std::string> dependsOn;

    bool operator==(const module_description& other) const
    {
        return name == other.name && dependsOn == other.dependsOn;
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["name"] = name;
        j["dependsOn"] = dependsOn;
        return j;
    }

    virtual std::string to_string() const = 0;
};

#endif //MODULEDESCRIPTION_HPP
