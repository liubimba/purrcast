//
// Created by bimba on 11/22/25.
//

#ifndef MODULEDESCRIPTION_HPP
#define MODULEDESCRIPTION_HPP
#include <string>
#include <vector>

struct module_description
{
    std::string name;
    std::vector<std::string> dependsOn;

    bool operator==(const module_description& other) const
    {
        return name == other.name && dependsOn == other.dependsOn;
    }
};

#endif //MODULEDESCRIPTION_HPP
