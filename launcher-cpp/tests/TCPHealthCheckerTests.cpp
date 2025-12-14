//
// Created by bimba on 11/22/25.
//

#include "Tests.hpp"

class Module : public IModule
{
public:
    bool m_loaded = true;

    bool load(const ModuleParams& params) override
    {
        return true;
    }

    bool reload(const ModuleParams& params) override
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

    ModuleParams getParams() const override
    {
        return {};
    }
};

TEST(TCPHealthChecker, status)
{
    Module module;
    module.m_loaded = false;
    std::initializer_list<int> ports = {1780, 1781};
    TCPHealthChecker checker(&module, TestData::services(), ports);
    ASSERT_EQ(checker.checkStatus(), HealthStatus::UNHEALTHY);
    module.m_loaded = true;
    ASSERT_EQ(checker.checkStatus(), HealthStatus::UNHEALTHY);
    std::vector<std::shared_ptr<Port>> boundPorts;
    for (auto it = ports.begin(); it != ports.end(); ++it)
    {
        boundPorts.push_back(Port::bind(*it));
        if (std::distance(it, ports.end()) == 1)
        {
            ASSERT_EQ(checker.checkStatus(), HealthStatus::HEALTHY);
        }
        else
        {
            ASSERT_EQ(checker.checkStatus(), HealthStatus::UNHEALTHY);
        }
    }

    for (auto& boundPort : boundPorts)
    {
        boundPort->release();
    }
}
