//
// Created by bimba on 11/15/25.
//

#ifndef PORT_HPP
#define PORT_HPP

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ansicolor_sink.h"
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class Port
{
    inline static std::shared_ptr<spdlog::logger> LOGGER_ = spdlog::create<
        spdlog::sinks::ansicolor_stdout_sink_mt>("Port");

    explicit Port(int port, int sockfd);

public:
    ~Port();
    static std::shared_ptr<Port> bind(int port);
    static bool isBound(int port);
    static bool invalid(int port);
    void release();

private:
    int port_;
    int sockfd_;
};


#endif //PORT_HPP
