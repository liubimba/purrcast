
//
// Created by bimba on 11/15/25.
//

#ifndef MODULEMANAGER_HPP
#define MODULEMANAGER_HPP
#include <memory>
#include <queue>
#include <unordered_set>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include "ModuleSession.hpp"
#include "../IModule.hpp"
#include "../../services/Services.hpp"

#include "../snapclient/SnapclientModule.hpp"
#include "../snapserver/SnapserverModule.hpp"
#include "../loopback/AudioLoopbackModule.hpp"
#include "../router/AudioRouterModule.hpp"
#include "../../health_checker/InternalHealthChecker.hpp"
#include "../../health_checker/TcpHealthChecker.hpp"


class ModuleManager
{
public:
    explicit ModuleManager(const Services* services);
    void add(const ModuleSession& session);
    void startup(const settings::s_manager& params);
    void shutdown();
    bool running();

private:
    void run_(settings::s_manager params);

    bool lookUpSessionsState_();
    bool dependentHealthy(const ModuleSession& session);
    bool tryLoad_(const ModuleSession& session);
    bool tryReload(const ModuleSession& session);
    void extraActions_(const ModuleSession& session);
    bool runningWithSameParameters_(const ModuleSession& session);

    boost::mutex mutex_;
    boost::thread thread_;
    const Services* services_;
    std::atomic_bool running_;
    boost::condition_variable condition_;
    std::shared_ptr<spdlog::logger> logger_;
    std::queue<std::function<void()>> taskQueue_;
    std::unordered_map<std::string, ModuleSession> sessions_;
};


#endif //MODULEMANAGER_HPP
