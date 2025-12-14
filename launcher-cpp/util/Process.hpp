//
// Created by bimba on 11/15/25.
//

#ifndef PROCESS_HPP
#define PROCESS_HPP
#include <string>

#include "IProcess.hpp"

class Process : public IProcess
{
public:
    Process(const Services* services, std::string id);
    ~Process() override;
    bool execute(const std::string& command, const std::string& arguments) override;
    void addListener(IProcessListener* listener) override;
    bool terminate() override;
    ProcessState state() override;

protected:
    std::unique_ptr<IProcess> instance_;
};


#endif //PROCESS_HPP
