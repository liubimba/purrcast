//
// Created by bimba on 11/15/25.
//

#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP
#include <memory>

#include "../logger/logger_factory.hpp"
#include "../modules/manager/module_manager.hpp"
#include "../services/services.hpp"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include "../modules/server/server_module.hpp"
#include "../services/pulse_audio_mainloop_service.hpp"

class launcher
{
public:
    launcher();
    void launch(const struct settings& params);
    void shutdown();

private:
    void launch_(const struct settings& settings, boost::asio::io_context& io_context);

    boost::mutex mutex_;
    boost::condition condition_;
    std::unique_ptr<module_manager> manager_;
};


#endif //LAUNCHER_HPP
