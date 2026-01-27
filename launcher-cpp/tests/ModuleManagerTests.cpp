//
// Created by bimba on 11/23/25.
//

#include "Tests.hpp"

TEST(ModuleManager, startup)
{
    module_params params = settings::s_module::loopback();

    auto d = module_cast(params);
    std::cout << d.module_name << std::endl;
}
