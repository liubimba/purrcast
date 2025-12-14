//
// Created by bimba on 11/15/25.
//

#ifndef IPROCESS_HPP
#define IPROCESS_HPP
#include <string>

#include "../services/Services.hpp"

enum class ProcessState
{
    NEW, EXECUTING, RUNNING, TERMINATING, TERMINATED, FAILED
};

class IProcessListener;

class IProcess
{
public:
    explicit IProcess(const Services* services, std::string id):
        services_(services), id_(std::move(id))
    {
    }

    virtual ~IProcess() = default;
    virtual bool terminate() = 0;
    virtual ProcessState state() = 0;
    virtual void addListener(IProcessListener* listener) = 0;
    virtual bool execute(const std::string& cmd, const std::string& args) = 0;

protected:
    std::string id_;
    const Services* services_;
};


class IProcessListener
{
public:
    virtual ~IProcessListener() = default;
    virtual void onTerminate() = 0;
};

#endif //IPROCESS_HPP
