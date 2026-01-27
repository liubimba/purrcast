//
// Created by bimba on 11/15/25.
//

#include "os_port.hpp"

os_port::os_port(int port, int sockfd):
    port_(port), sockfd_(sockfd)
{
}

os_port::~os_port()
{
    release();
}

std::shared_ptr<os_port> os_port::bind(int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        LOGGER_->error("Could not create socket");
        return nullptr;
    }
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (::bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(sockfd);
        if (errno == EADDRINUSE)
        {
            LOGGER_->error("Port {} is bound", port);
            return nullptr;
        }
        LOGGER_->error("Failed to bind port {}. Error: ", port, strerror(errno));
        return nullptr;
    }
    LOGGER_->info("Bound port {}", port);
    return std::shared_ptr<os_port>(new os_port{port, sockfd});
}

bool os_port::is_bound(int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        LOGGER_->error("Could not create socket");
        return false;
    }
    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&option, sizeof(option));
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (::bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(sockfd);
        if (errno == EADDRINUSE)
        {
            return true;
        }
        return false;
    }
    close(sockfd);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return false;
}

bool os_port::invalid(int port)
{
    return port <= 0;
}

void os_port::release()
{
    LOGGER_->info("Release port {}", port_);
    if (sockfd_ >= 0)
    {
        close(sockfd_);
    }
    sockfd_ = -1;
    port_ = -1;
}
