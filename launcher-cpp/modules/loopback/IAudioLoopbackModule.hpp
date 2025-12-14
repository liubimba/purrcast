//
// Created by bimba on 11/23/25.
//

#ifndef IAUDIOLOOPBACKMODULE_HPP
#define IAUDIOLOOPBACKMODULE_HPP
#include "../IModule.hpp"

class IAudioLoopbackModule : public IModule
{
public:
    ~IAudioLoopbackModule() = default;

    virtual uint32_t getSinkIndex() = 0;
    virtual std::string getMonitorDescription() = 0;
};

#endif //IAUDIOLOOPBACKMODULE_HPP
