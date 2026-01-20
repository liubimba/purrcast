
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
#include "../ModuleStatus.hpp"

class ModuleManager
{
public:
    explicit ModuleManager(const Services* services);
    void add(const module_session& session);
    void startup(const settings::s_manager& params);
    void shutdown();
    bool running();
    std::vector<ModuleStatus> report();

private:
    void run_(settings::s_manager params);

    bool look_up_();
    bool ready_by_dependency(const module_session& session);
    bool ready_by_order(const module_session& session);
    bool try_load_(const module_session& session);
    bool try_reload_(const module_session& session);
    void extra_(const module_session& session);
    bool is_running_with_same_params_(const module_session& session);

    std::vector<std::string> get_unhealthy_dependent_modules_by_order(const module_session& session);
    boost::mutex mutex_;
    boost::thread thread_;
    const Services* services_;
    std::atomic_bool running_;
    boost::condition_variable condition_;
    std::shared_ptr<spdlog::logger> logger_;
    std::queue<std::function<void()>> taskQueue_;
    std::unordered_map<std::string, module_session> sessions_;
};


#endif //MODULEMANAGER_HPP
