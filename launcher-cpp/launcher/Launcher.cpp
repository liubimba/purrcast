//
// Created by bimba on 11/15/25.
//

#include "Launcher.hpp"

#include "../modules/monitor/MonitorModule.hpp"
#include "../modules/test_environment/TestEnvironmentModule.hpp"


Launcher::Launcher()
= default;

void Launcher::launch(const struct settings& params)
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

void Launcher::shutdown()
{
    spdlog::info("Shutdown");
    condition_.notify_all();
}

void Launcher::launch_(const settings& settings, boost::asio::io_context& io_context)
{
    Services services;
    services.add(std::make_shared<LoggerFactory>());
    services.add(std::make_shared<pulse::MainloopService>(&services));

    manager_ = std::make_unique<ModuleManager>(&services);

    AudioRouterModule router_module(&services);
    InternalHealthChecker routerChecker(&router_module, &services);
    module_session audio_router_session;
    audio_router_session.checker = &routerChecker;
    audio_router_session.module = &router_module;
    audio_router_session.params = settings.module.router;

    AudioLoopbackModule loopbackModule(&services);
    InternalHealthChecker loopbackChecker(&loopbackModule, &services);
    module_session audio_loopback_session;
    audio_loopback_session.checker = &loopbackChecker;
    audio_loopback_session.module = &loopbackModule;
    audio_loopback_session.params = settings.module.loopback;

    SnapclientModule snapclientModule(&services);
    InternalHealthChecker snapclientChecker(&snapclientModule, &services);
    module_session snapclient_session;
    snapclient_session.checker = &snapclientChecker;
    snapclient_session.module = &snapclientModule;
    snapclient_session.params = settings.module.snapclient;

    SnapserverModule snapserverModule(&services);
    TCPHealthChecker snapserverChecker(&snapserverModule, &services, {
                                           settings.module.snapserver.ports.control,
                                           settings.module.snapserver.ports.http,
                                           settings.module.snapserver.ports.stream
                                       });
    module_session snapserver_session;
    snapserver_session.checker = &snapserverChecker;
    snapserver_session.module = &snapserverModule;
    snapserver_session.params = settings.module.snapserver;

    ServerModule serverModule(&services);
    TCPHealthChecker serverChecker(&serverModule, &services, {
                                       settings.module.server.port
                                   });
    module_session http_server_session;
    http_server_session.checker = &serverChecker;
    http_server_session.module = &serverModule;
    http_server_session.params = settings.module.server;


    MonitorModule monitorServer{io_context, manager_.get(), &services};
    TCPHealthChecker monitorChecker(&monitorServer, &services, {
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

    AudioLoopbackModule test_loopback_module(&services);
    InternalHealthChecker test_loopback_checker(&test_loopback_module, &services);
    module_session test_loopback_session;
    test_loopback_session.checker = &test_loopback_checker;
    test_loopback_session.module = &test_loopback_module;
    test_loopback_session.params = settings.module.test_environment.loopback;

    SnapclientModule test_snapclient_module(&services);
    InternalHealthChecker test_snapclient_checker(&snapclientModule, &services);
    module_session test_snapclient_session;
    test_snapclient_session.checker = &test_snapclient_checker;
    test_snapclient_session.module = &test_snapclient_module;
    test_snapclient_session.params = settings.module.test_environment.snapclient;

    TestEnvironmentModule test_environment_module{io_context, &services, router_module.get_source()};
    InternalHealthChecker test_environment_checker(&test_environment_module, &services);
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
