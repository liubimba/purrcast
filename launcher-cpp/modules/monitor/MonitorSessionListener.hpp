//
// Created by bimba on 12/25/25.
//

#ifndef IPCSESSIONLISTENER_HPP
#define IPCSESSIONLISTENER_HPP

#include <string>
#include <memory>

class MonitorSession;

class MonitorSessionListener
{
public:
    virtual ~MonitorSessionListener() = default;
    virtual void onMessage(std::shared_ptr<MonitorSession> session, const std::string& message) = 0;
    virtual void onClose(std::shared_ptr<MonitorSession> session, const std::string& reason) = 0;
};

#endif //IPCSESSIONLISTENER_HPP
