//
// Created by bimba on 11/22/25.
//

#include "Tests.hpp"

class Module : public IModule
{
public:
    bool m_loaded = false;

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

    bool loaded() const override
    {
        return m_loaded;
    }

    std::string name() const override
    {
        return TestData::uuid();
    }

    ModuleParams getParams() const override
    {
        return {};
    }
};

TEST(InternalHealthChecker, status)
{
    Module module;
    module.m_loaded = true;
    InternalHealthChecker checker(&module, TestData::services());
    ASSERT_EQ(checker.checkStatus(), HealthStatus::HEALTHY);
    module.m_loaded = false;
    ASSERT_EQ(checker.checkStatus(), HealthStatus::UNHEALTHY);
}
