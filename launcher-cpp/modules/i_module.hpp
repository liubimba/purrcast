//
// Created by bimba on 11/15/25.
//

#ifndef IMODULE_HPP
#define IMODULE_HPP
#include <string>

#include "module_status.hpp"
#include "../settings/settings.hpp"

class i_module
{
protected:
    module_status last_status_ = module_status::pending();

    void set_last_status_(const module_status& last_status)
    {
        last_status_ = last_status;
    }

public:
    virtual ~i_module() = default;
    virtual bool load(const module_params& params) = 0;
    virtual bool reload(const module_params& params) = 0;
    virtual bool unload() = 0;
    [[nodiscard]] virtual bool loaded() const = 0;
    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual module_params get_params() const = 0;

    [[nodiscard]] virtual module_status get_last_status() const
    {
        return last_status_;
    }
};

#endif //IMODULE_HPP
