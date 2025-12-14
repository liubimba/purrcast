//
// Created by bimba on 11/23/25.
//

#include "Tests.hpp"

TEST(ModuleManager, startup)
{
    ModuleParams params = settings::s_module::loopback();

    auto d = module_cast(params);
    std::cout << d.name << std::endl;
}
