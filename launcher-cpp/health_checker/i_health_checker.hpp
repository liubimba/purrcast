//
// Created by bimba on 11/22/25.
//

#ifndef IHEALTHCHECKER_HPP
#define IHEALTHCHECKER_HPP
#include "health_status.hpp"
#include "../modules/i_module.hpp"


class i_health_checker
{
public:
    explicit i_health_checker(const i_module* module):
        module_(module)
    {
    }

    virtual ~i_health_checker() = default;
    [[nodiscard]] virtual health_status check() const = 0;

protected:
    const i_module* module_;
};

#endif //IHEALTHCHECKER_HPP
