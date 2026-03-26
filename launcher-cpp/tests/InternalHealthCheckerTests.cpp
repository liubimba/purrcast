//
// Created by bimba on 11/22/25.
//

#include "Tests.hpp"

class Module : public i_module
{
public:
    bool m_loaded = false;

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

    bool loaded() const override
    {
        return m_loaded;
    }

    std::string name() const override
    {
        return TestData::uuid();
    }

    module_params get_params() const override
    {
        return {};
    }
};

TEST(InternalHealthChecker, status)
{
    Module module;
    module.m_loaded = true;
    internal_health_checker checker(&module, TestData::get_services());
    ASSERT_EQ(checker.check(), health_status::healthy);
    module.m_loaded = false;
    ASSERT_EQ(checker.check(), health_status::unhealthy);
}
