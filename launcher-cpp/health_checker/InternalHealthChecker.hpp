//
// Created by bimba on 11/22/25.
//

#ifndef INTERNALHEALTHCHECKER_HPP
#define INTERNALHEALTHCHECKER_HPP
#include "IHealthChecker.hpp"
#include "../services/Services.hpp"


class InternalHealthChecker : public IHealthChecker
{
public:
    explicit InternalHealthChecker(const IModule* module, const Services* services);
    ~InternalHealthChecker() override;
    HealthStatus checkStatus() const override;

private:
    const Services* services_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //INTERNALHEALTHCHECKER_HPP
