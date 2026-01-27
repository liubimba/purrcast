//
// Created by bimba on 11/22/25.
//

#ifndef TCPHEALTHCHECKER_HPP
#define TCPHEALTHCHECKER_HPP
#include <unordered_set>

#include "internal_health_checker.hpp"
#include "../logger/logger_factory.hpp"
#include "../util/os_port.hpp"


class tcp_health_checker : public internal_health_checker
{
public:
    explicit tcp_health_checker(const i_module* module, const services* services, std::initializer_list<int> ports);
    ~tcp_health_checker() override;
    health_status check() const override;

private:
    const services* services_;
    std::unordered_set<int> ports_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //TCPHEALTHCHECKER_HPP
