//
// Created by bimba on 11/22/25.
//

#ifndef MODULEDESCRIPTION_HPP
#define MODULEDESCRIPTION_HPP
#include <string>
#include <vector>

#include "../libs/snapcast/common/json.hpp"

#define MAX_ORDER 1 << 32
#define MIN_ORDER 0

struct module_description
{
    virtual ~module_description() = default;
    int order_of_loading = MAX_ORDER;
    bool enabled = true;
    std::string module_name;
    std::vector<std::string> depends_on;

    bool operator==(const module_description& other) const
    {
        return module_name == other.module_name && depends_on == other.depends_on;
    }

    [[nodiscard]] nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["module_name"] = module_name;
        j["depends_on"] = depends_on;
        j["order_of_loading"] = order_of_loading;
        return j;
    }

    [[nodiscard]] virtual std::string to_string() const = 0;
};

#endif //MODULEDESCRIPTION_HPP
