//
// Created by bimba on 11/15/25.
//

#ifndef PROCESS_HPP
#define PROCESS_HPP
#include <string>
#include <utility>

#include "linux_process.hpp"
#include "../logger/logger_factory.hpp"
#include "i_process.hpp"

class os_process : public i_process
{
public:
    os_process(const services* services, std::string id);
    ~os_process() override;
    process_result execute(const std::string& command, const std::string& arguments) override;
    void add_listener(i_process_listener* listener) override;
    process_result terminate() override;
    process_state state() override;

protected:
    std::unique_ptr<i_process> instance_;
};


#endif //PROCESS_HPP
