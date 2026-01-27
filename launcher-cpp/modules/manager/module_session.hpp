//
// Created by bimba on 11/22/25.
//

#ifndef MODULESESSION_HPP
#define MODULESESSION_HPP
#include <memory>

#include "../i_module.hpp"
#include "../../health_checker/i_health_checker.hpp"

typedef struct module_session
{
    module_params params;
    i_module* module;
    i_health_checker* checker;

    bool operator==(const module_session& other) const
    {
        return params == other.params && module == other.module && checker == other.checker;
    }
} module_session;

#endif //MODULESESSION_HPP
