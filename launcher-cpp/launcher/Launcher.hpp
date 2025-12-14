//
// Created by bimba on 11/15/25.
//

#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP
#include <memory>

#include "../logger/LoggerFactory.hpp"
#include "../modules/manager/ModuleManager.hpp"
#include "../services/Services.hpp"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include "../modules/server/ServerModule.hpp"
#include "../services/PulseMainloopService.hpp"

class Launcher
{
public:
    Launcher();
    void launch(const struct settings& params);
    void shutdown();

private:
    void launch_(const struct settings& settings);

    boost::mutex mutex_;
    boost::condition condition_;
    std::unique_ptr<ModuleManager> manager_;
};


#endif //LAUNCHER_HPP
