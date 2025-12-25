//
// Created by bimba on 12/25/25.
//

#ifndef MONITORREPORTMESSAGE_HPP
#define MONITORREPORTMESSAGE_HPP
#include <string>
#include <vector>

#include "../ModuleStatus.hpp"
#include "absl/strings/str_join.h"

namespace message
{
    namespace monitor
    {
        inline std::string report(std::vector<ModuleStatus> report)
        {
            std::vector<std::string> transformed;
            std::transform(report.begin(), report.end(), std::back_inserter(transformed), [](const ModuleStatus& ms)
            {
                nlohmann::json j;
                j["code"] = ms.code;
                j["severity"] = ms.severity;
                j["advice"] = ms.advice;
                j["message"] = ms.message;
                j["timestampNs"] = ms.timestampNs;
                j["module"] = std::visit([](const auto& p)
                {
                    return p.to_string();
                }, ms.params);
                return j.dump();
            });
            std::string payload = absl::StrFormat("[%s]", absl::StrJoin(transformed, ","));

            nlohmann::json j = {
                {"type", "report"},
                {"payload", payload}
            };
            return j.dump();
        }
    }
}

#endif //MONITORREPORTMESSAGE_HPP
