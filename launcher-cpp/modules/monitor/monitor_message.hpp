//
// Created by bimba on 12/25/25.
//

#ifndef MONITORREPORTMESSAGE_HPP
#define MONITORREPORTMESSAGE_HPP
#include <string>
#include <vector>

#include "../module_status.hpp"
#include "absl/strings/str_join.h"


namespace message::monitor
{
    inline std::string report(const std::vector<module_session>& sessions)
    {
        std::vector<std::string> transformed;
        std::transform(sessions.begin(), sessions.end(), std::back_inserter(transformed), [](const module_session& module_session)
        {
            nlohmann::json json;
            json["module_name"] = module_session.module->name();
            json["health_checker"] = module_session.checker->check();
            json["module_status"] = module_session.module->get_last_status().to_json().dump();
            return json.dump();
        });
        std::string payload = absl::StrFormat("[%s]", absl::StrJoin(transformed, ","));

        nlohmann::json j = {
            {"type", "report"},
            {"payload", payload}
        };
        return j.dump();
    }
}


#endif //MONITORREPORTMESSAGE_HPP
