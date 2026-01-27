//
// Created by bimba on 11/22/25.
//

#include "Tests.hpp"

class Module : public i_module
{
public:
    bool m_loaded = true;

    bool load(const module_params& params) override
    {
        return true;
    }

    bool reload(const module_params& params) override
    {
        return true;
    }

    bool unload() override
    {
        return true;
    }

    [[nodiscard]] bool loaded() const override
    {
        return m_loaded;
    }

    [[nodiscard]] std::string name() const override
    {
        return TestData::uuid();
    }

    module_params get_params() const override
    {
        return {};
    }
};

TEST(TCPHealthChecker, status)
{
    Module module;
    module.m_loaded = false;
    std::initializer_list<int> ports = {1780, 1781};
    tcp_health_checker checker(&module, TestData::services(), ports);
    ASSERT_EQ(checker.check(), health_status::unhealthy);
    module.m_loaded = true;
    ASSERT_EQ(checker.check(), health_status::unhealthy);
    std::vector<std::shared_ptr<os_port>> boundPorts;
    for (auto it = ports.begin(); it != ports.end(); ++it)
    {
        boundPorts.push_back(os_port::bind(*it));
        if (std::distance(it, ports.end()) == 1)
        {
            ASSERT_EQ(checker.check(), health_status::healthy);
        }
        else
        {
            ASSERT_EQ(checker.check(), health_status::unhealthy);
        }
    }

    for (auto& boundPort : boundPorts)
    {
        boundPort->release();
    }
}
