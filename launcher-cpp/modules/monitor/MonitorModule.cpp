//
// Created by bimba on 12/21/25.
//

#include "MonitorModule.hpp"

#include <iostream>

#include "MonitorReportMessage.hpp"
#include "../../libs/snapcast/common/json.hpp"
#include "absl/strings/str_join.h"

MonitorModule::MonitorModule(boost::asio::io_context& io_context, ModuleManager* module_manager,
                             const Services* services):
    services_(services), io_context_(io_context), loaded_(false),
    module_manager_(module_manager), timer_(io_context),
    strand_(boost::asio::make_strand(io_context))
{
    logger_ = services->get<LoggerFactory>()->create("MonitorServer");
}

MonitorModule::~MonitorModule()
{
    spdlog::drop("MonitoServer");
}

void MonitorModule::accept_()
{
    acceptor_->async_accept(
        strand_,
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            if (!acceptor_->is_open()) return;
            if (ec)
            {
                logger_->error(ec.message());
                return;
            }
            auto remoteEndpoint = socket.remote_endpoint(ec);
            if (ec) return;

            std::string id = absl::StrFormat("%s:%d", remoteEndpoint.address().to_string(),
                                             remoteEndpoint.port());
            logger_->info("Accept new connection: {}", id);
            std::shared_ptr<MonitorSession> session = std::make_shared<MonitorSession>(this, std::move(socket), id);
            session->start();

            sessions_.emplace_back(std::move(session));

            accept_();
        });
}

void MonitorModule::notify_()
{
    std::vector<ModuleStatus> report = module_manager_->report();
    std::string message = ::message::monitor::report(std::move(report));
    std::for_each(sessions_.begin(), sessions_.end(), [message](const std::shared_ptr<MonitorSession>& session)
    {
        session->send(message);
    });
}

void MonitorModule::schedule_()
{
    timer_.expires_after(std::chrono::milliseconds(params_.interval));
    timer_.async_wait(boost::asio::bind_executor(strand_, [this](boost::system::error_code ec)
    {
        if (ec || !loaded_) return;
        notify_();
        schedule_();
    }));
}

bool MonitorModule::load(const ModuleParams& moduleParams)
{
    if (loaded_) throw std::runtime_error("MonitorServer already loaded");
    settings::s_module::s_monitor params = std::get<settings::s_module::s_monitor>(moduleParams);

    try
    {
        settings::s_module::s_monitor params =
            std::get<settings::s_module::s_monitor>(moduleParams);
        if (params.interval <= 0) throw std::invalid_argument("interval must be greater than zero");
        boost::system::error_code ec;
        boost::asio::ip::address::from_string(params.address, ec);
        if (ec) throw std::invalid_argument("invalid address");
        if (params.port <= 0) throw std::invalid_argument("port must be greater than zero");

        boost::asio::ip::tcp::endpoint ep(
            boost::asio::ip::make_address(params.address),
            params.port
        );

        acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(
            boost::asio::make_strand(io_context_)
        );

        acceptor_->open(ep.protocol());
        acceptor_->set_option(boost::asio::socket_base::reuse_address(true));
        acceptor_->bind(ep);
        acceptor_->listen();

        accept_();
        schedule_();

        params_ = params;
        loaded_ = true;
    }
    catch (const std::exception& e)
    {
        logger_->error("Failed to load module. Error: {}", e.what());
        loaded_ = false;
    }

    return loaded_;
}

bool MonitorModule::reload(const ModuleParams& moduleParams)
{
    if (!loaded_)
        throw std::runtime_error("Unavailable reload unloaded MonitorServer");
    if (!std::holds_alternative<settings::s_module::s_monitor>(moduleParams))
        throw std::invalid_argument("invalid module parameters");
    settings::s_module::s_monitor params = std::get<settings::s_module::s_monitor>(moduleParams);
    if (params == params_)
    {
        logger_->info("Passed same module parameters for reloading, pass this request");
        return false;
    }

    return load(moduleParams);
}

bool MonitorModule::unload()
{
    if (!loaded_) throw std::runtime_error("Unavailable unload unloaded MonitorServer");
    logger_->info("Unloading monitor server");
    std::promise<void> unloaded;
    std::future<void> future = unloaded.get_future();
    boost::asio::dispatch(strand_, [&]()
    {
        timer_.cancel();

        if (acceptor_ && acceptor_->is_open())
            acceptor_->close();

        std::for_each(sessions_.begin(), sessions_.end(), [this](const std::shared_ptr<MonitorSession>& session)
        {
            logger_->info("Closing connection: {}", session->get_id());
            websocket::close_reason reason;
            reason.code = websocket::close_code::normal;
            reason.reason = "normal";
            session->close(reason);
        });
        sessions_.clear();

        unloaded.set_value();
        loaded_ = false;
    });
    if (!strand_.running_in_this_thread())
        future.wait();

    return !loaded_;
}

bool MonitorModule::loaded() const
{
    return loaded_;
}

std::string MonitorModule::name() const
{
    return params_.name;
}

ModuleParams MonitorModule::get_params() const
{
    return params_;
}

void MonitorModule::onMessage(std::shared_ptr<MonitorSession> session, const std::string& message)
{
    // TODO
}

void MonitorModule::onClose(std::shared_ptr<MonitorSession> session, const std::string& reason)
{
    boost::asio::post(strand_, [this, id = session->get_id()]
    {
        sessions_.erase(
            std::remove_if(sessions_.begin(), sessions_.end(),
                           [this, id](const std::shared_ptr<MonitorSession>& s)
                           {
                               if (s->get_id() == id)
                                   logger_->info("Close connection: {}", id);
                               return s->get_id() == id;
                           }),
            sessions_.end()
        );
    });
}
