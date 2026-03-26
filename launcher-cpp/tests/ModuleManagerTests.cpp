//
// Created by bimba on 11/23/25.
//

#include "Tests.hpp"

TEST(ModuleManager, startup)
{
    settings::s_module::s_loopback params{};
    const auto& d = module_cast(params);
    std::cout << d.module_name << std::endl;
}
