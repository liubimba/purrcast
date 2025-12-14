//
// Created by bimba on 11/15/25.
//

#ifndef IMODULE_HPP
#define IMODULE_HPP
#include <string>

#include "../settings/Settings.hpp"

class IModule
{
public:
    virtual ~IModule() = default;
    virtual bool load(const ModuleParams& params) = 0;
    virtual bool reload(const ModuleParams& params) = 0;
    virtual bool unload() = 0;
    virtual bool loaded() const = 0;
    virtual std::string name() const = 0;
    virtual ModuleParams getParams() const = 0;
};

#endif //IMODULE_HPP
