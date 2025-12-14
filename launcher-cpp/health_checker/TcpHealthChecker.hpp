//
// Created by bimba on 11/22/25.
//

#ifndef TCPHEALTHCHECKER_HPP
#define TCPHEALTHCHECKER_HPP
#include <unordered_set>

#include "InternalHealthChecker.hpp"
#include "../logger/LoggerFactory.hpp"
#include "../util/Port.hpp"


class TCPHealthChecker : public InternalHealthChecker
{
public:
    explicit TCPHealthChecker(const IModule* module, const Services* services, std::initializer_list<int> ports);
    ~TCPHealthChecker() override;
    HealthStatus checkStatus() const override;

private:
    const Services* services_;
    std::unordered_set<int> ports_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //TCPHEALTHCHECKER_HPP
