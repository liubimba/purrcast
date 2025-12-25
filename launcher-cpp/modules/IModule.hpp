//
// Created by bimba on 11/15/25.
//

#ifndef IMODULE_HPP
#define IMODULE_HPP
#include <string>

#include "ModuleStatus.hpp"
#include "../settings/Settings.hpp"

class IModule
{
protected:
    ModuleStatus status_ = {};

public:
    virtual ~IModule() = default;
    virtual bool load(const ModuleParams& params) = 0;
    virtual bool reload(const ModuleParams& params) = 0;
    virtual bool unload() = 0;
    [[nodiscard]] virtual bool loaded() const = 0;
    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual ModuleParams get_params() const = 0;

    [[nodiscard]] virtual ModuleStatus get_last_status() const
    {
        return status_;
    }
};

#endif //IMODULE_HPP
