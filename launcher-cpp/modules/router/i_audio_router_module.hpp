//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOROUTERMODULE_HPP
#define IAUDIOROUTERMODULE_HPP
#include "../i_module.hpp"

class IAudioSource;

class i_audio_router_module : public i_module
{
public:
     ~i_audio_router_module() override = default;

     [[nodiscard]] virtual IAudioSource* get_source() const = 0;
};

#endif //IAUDIOROUTERMODULE_HPP
