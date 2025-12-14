//
// Created by bimba on 11/22/25.
//

#ifndef MODULESESSION_HPP
#define MODULESESSION_HPP
#include <memory>

#include "../IModule.hpp"
#include "../../health_checker/IHealthChecker.hpp"

typedef struct ModuleSession
{
    ModuleParams params;
    IModule* module;
    IHealthChecker* checker;

    bool operator==(const ModuleSession& other) const
    {
        return params == other.params && module == other.module && checker == other.checker;
    }
} ModuleSession;

#endif //MODULESESSION_HPP
