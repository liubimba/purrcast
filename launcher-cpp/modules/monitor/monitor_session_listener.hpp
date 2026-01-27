//
// Created by bimba on 12/25/25.
//

#ifndef IPCSESSIONLISTENER_HPP
#define IPCSESSIONLISTENER_HPP

#include <string>
#include <memory>

class monitor_session;

class monitor_session_listener
{
public:
    virtual ~monitor_session_listener() = default;
    virtual void on_message(std::shared_ptr<monitor_session> session, const std::string& message) = 0;
    virtual void on_close(std::shared_ptr<monitor_session> session, const std::string& reason) = 0;
};

#endif //IPCSESSIONLISTENER_HPP
