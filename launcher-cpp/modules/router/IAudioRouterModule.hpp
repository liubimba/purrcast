//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOROUTERMODULE_HPP
#define IAUDIOROUTERMODULE_HPP
#include "../IModule.hpp"

class IAudioSource;

class IAudioRouterModule : public IModule
{
public:
     ~IAudioRouterModule() override = default;

     [[nodiscard]] virtual IAudioSource* get_source() const = 0;
};

#endif //IAUDIOROUTERMODULE_HPP
