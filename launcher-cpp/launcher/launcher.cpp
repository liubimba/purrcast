//
// Created by bimba on 11/15/25.
//

#include "launcher.hpp"

#include "../modules/monitor/monitor_module.hpp"
#include "../modules/test_environment/test_environment.hpp"


launcher::launcher()
= default;

void launcher::launch(const settings& params)
{
    spdlog::info("Launch");
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM, SIGQUIT);
    signals.async_wait([&](const boost::system::error_code&, int)
    {
        condition_.notify_all();
    });
    boost::thread t([&]
    {
        launch_(params, io_context);
        io_context.stop();
    });
    io_context.run();
    t.join();
}

void launcher::shutdown()
{
    spdlog::info("App caught signal to shutdown");
    condition_.notify_all();
}

void launcher::launch_(const settings& settings, boost::asio::io_context& io_context)
{
    services services;
    services.add(std::make_shared<logger_factory>());
    services.add(std::make_shared<pulse::mainloop_service>(&services));

    manager_ = std::make_unique<module_manager>(&services);

    audio_router_module router_module(&services);
    internal_health_checker routerChecker(&router_module, &services);
    module_session audio_router_session;
    audio_router_session.checker = &routerChecker;
    audio_router_session.module = &router_module;
    audio_router_session.params = settings.module.router;

    audio_loopback_module loopbackModule(&services);
    internal_health_checker loopbackChecker(&loopbackModule, &services);
    module_session audio_loopback_session;
    audio_loopback_session.checker = &loopbackChecker;
    audio_loopback_session.module = &loopbackModule;
    audio_loopback_session.params = settings.module.loopback;

    snapclient_module snapclientModule(&services);
    internal_health_checker snapclientChecker(&snapclientModule, &services);
    module_session snapclient_session;
    snapclient_session.checker = &snapclientChecker;
    snapclient_session.module = &snapclientModule;
    snapclient_session.params = settings.module.snapclient;

    snapserver_module snapserverModule(&services);
    tcp_health_checker snapserverChecker(&snapserverModule, &services, {
                                             settings.module.snapserver.ports.control,
                                             settings.module.snapserver.ports.http,
                                             settings.module.snapserver.ports.stream
                                         });
    module_session snapserver_session;
    snapserver_session.checker = &snapserverChecker;
    snapserver_session.module = &snapserverModule;
    snapserver_session.params = settings.module.snapserver;

    server_module serverModule(&services);
    tcp_health_checker serverChecker(&serverModule, &services, {
                                         settings.module.server.port
                                     });
    module_session http_server_session;
    http_server_session.checker = &serverChecker;
    http_server_session.module = &serverModule;
    http_server_session.params = settings.module.server;


    monitor_module monitorServer{io_context, manager_.get(), &services};
    tcp_health_checker monitorChecker(&monitorServer, &services, {
                                          settings.module.monitor.port
                                      });
    module_session ws_monitor_session;
    ws_monitor_session.checker = &monitorChecker;
    ws_monitor_session.module = &monitorServer;
    ws_monitor_session.params = settings.module.monitor;

    manager_->add(snapserver_session);
    manager_->add(snapclient_session);
    manager_->add(http_server_session);
    manager_->add(ws_monitor_session);
    manager_->add(audio_router_session);
    manager_->add(audio_loopback_session);

    audio_loopback_module test_loopback_module(&services);
    internal_health_checker test_loopback_checker(&test_loopback_module, &services);
    module_session test_loopback_session;
    test_loopback_session.checker = &test_loopback_checker;
    test_loopback_session.module = &test_loopback_module;
    test_loopback_session.params = settings.module.test_environment.loopback;

    snapclient_module test_snapclient_module(&services);
    internal_health_checker test_snapclient_checker(&snapclientModule, &services);
    module_session test_snapclient_session;
    test_snapclient_session.checker = &test_snapclient_checker;
    test_snapclient_session.module = &test_snapclient_module;
    test_snapclient_session.params = settings.module.test_environment.snapclient;

    test_environment test_environment_module{io_context, &services, router_module.get_source()};
    internal_health_checker test_environment_checker(&test_environment_module, &services);
    module_session test_environment_session;
    test_environment_session.checker = &test_environment_checker;
    test_environment_session.module = &test_environment_module;
    test_environment_session.params = settings.module.test_environment;

    if (settings.module.test_environment.enabled)
    {
        manager_->add(test_environment_session);
        manager_->add(test_snapclient_session);
        manager_->add(test_loopback_session);
    }

    manager_->startup(settings.manager);

    boost::unique_lock lock(mutex_);
    condition_.wait(lock);

    manager_->shutdown();
}
