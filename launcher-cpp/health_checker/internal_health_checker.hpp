//
// Created by bimba on 11/22/25.
//

#ifndef INTERNALHEALTHCHECKER_HPP
#define INTERNALHEALTHCHECKER_HPP
#include "i_health_checker.hpp"
#include "../services/services.hpp"


class internal_health_checker : public i_health_checker
{
public:
    explicit internal_health_checker(const i_module* module, const services* services);
    ~internal_health_checker() override;
    [[nodiscard]] health_status check() const override;

private:
    const services* services_;
    std::shared_ptr<spdlog::logger> logger_;
};

#endif //INTERNALHEALTHCHECKER_HPP
