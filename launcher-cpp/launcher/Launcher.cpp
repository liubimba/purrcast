//
// Created by bimba on 11/15/25.
//

#include "Launcher.hpp"


Launcher::Launcher()
= default;

void Launcher::launch(const struct settings& params)
{
    spdlog::info("Launch");
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM, SIGQUIT);
    signals.async_wait([&](const boost::system::error_code&, int)
    {
        this->shutdown();
        io_context.stop();
    });
    boost::thread t([&]
    {
        this->launch_(params);
    });
    io_context.run();
    t.join();
}

void Launcher::shutdown()
{
    spdlog::info("Shutdown");
    manager_->shutdown();
    condition_.notify_all();
}

void Launcher::launch_(const settings& settings)
{
    Services services;
    services.add(std::make_shared<LoggerFactory>());
    services.add(std::make_shared<pulse::MainloopService>(&services));

    AudioRouterModule routerModule(&services);
    InternalHealthChecker routerChecker(&routerModule, &services);
    ModuleSession routerSession;
    routerSession.checker = &routerChecker;
    routerSession.module = &routerModule;
    routerSession.params = settings.module.router;

    AudioLoopbackModule loopbackModule(&services);
    InternalHealthChecker loopbackChecker(&loopbackModule, &services);
    ModuleSession loopbackSession;
    loopbackSession.checker = &loopbackChecker;
    loopbackSession.module = &loopbackModule;
    loopbackSession.params = settings.module.loopback;

    SnapclientModule snapclientModule(&services);
    InternalHealthChecker snapclientChecker(&snapclientModule, &services);
    ModuleSession snapclientSession;
    snapclientSession.checker = &snapclientChecker;
    snapclientSession.module = &snapclientModule;
    snapclientSession.params = settings.module.snapclient;

    SnapserverModule snapserverModule(&services);
    TCPHealthChecker snapserverChecker(&snapserverModule, &services, {
                                           settings.module.snapserver.ports.control,
                                           settings.module.snapserver.ports.http,
                                           settings.module.snapserver.ports.stream
                                       });
    ModuleSession snapserverSession;
    snapserverSession.checker = &snapserverChecker;
    snapserverSession.module = &snapserverModule;
    snapserverSession.params = settings.module.snapserver;

    ServerModule serverModule(&services);
    TCPHealthChecker serverChecker(&serverModule, &services, {
                                       settings.module.server.port
                                   });
    ModuleSession serverSession;
    serverSession.checker = &serverChecker;
    serverSession.module = &serverModule;
    serverSession.params = settings.module.server;


    manager_ = std::make_unique<ModuleManager>(&services);
    manager_->add(routerSession);
    manager_->add(loopbackSession);
    manager_->add(snapserverSession);
    manager_->add(snapclientSession);
    manager_->add(serverSession);

    manager_->startup(settings.manager);

    boost::unique_lock lock(mutex_);
    condition_.wait(lock);
}
