//
// Created by bimba on 11/22/25.
//

#ifndef IHEALTHCHECKER_HPP
#define IHEALTHCHECKER_HPP
#include "HealthStatus.hpp"
#include "../modules/IModule.hpp"


class IHealthChecker
{
public:
    explicit IHealthChecker(const IModule* module):
        module_(module)
    {
    }

    virtual ~IHealthChecker() = default;
    virtual HealthStatus checkStatus() const = 0;

protected:
    const IModule* module_;
};

#endif //IHEALTHCHECKER_HPP
