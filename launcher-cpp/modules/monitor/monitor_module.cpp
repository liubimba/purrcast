//
// Created by bimba on 12/21/25.
//

#include "monitor_module.hpp"

#include <iostream>

#include "monitor_message.hpp"
#include "../../libs/snapcast/common/json.hpp"
#include "../test_environment/test_environment.hpp"
#include "absl/strings/str_join.h"

monitor_module::monitor_module(boost::asio::io_context& io_context, module_manager* module_manager,
                               const services* services):
    services_(services), io_context_(io_context), loaded_(false),
    module_manager_(module_manager), timer_(io_context),
    strand_(boost::asio::make_strand(io_context))
{
    logger_ = services->get<logger_factory>()->create("MonitorServer");
}

monitor_module::~monitor_module()
{
    spdlog::drop("MonitoServer");
}

void monitor_module::accept_()
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
            std::shared_ptr<monitor_session> session = std::make_shared<monitor_session>(this, std::move(socket), id);
            session->start();

            sessions_.emplace_back(std::move(session));

            accept_();
        });
}

void monitor_module::notify_()
{
    std::string message = ::message::monitor::report(module_manager_->get_sessions());
    std::for_each(sessions_.begin(), sessions_.end(), [message](const std::shared_ptr<monitor_session>& session)
    {
        session->send(message);
    });
}

void monitor_module::schedule_()
{
    timer_.expires_after(std::chrono::milliseconds(params_.interval));
    timer_.async_wait(boost::asio::bind_executor(strand_, [this](boost::system::error_code ec)
    {
        if (ec || !loaded_) return;
        notify_();
        schedule_();
    }));
}

bool monitor_module::load(const module_params& module_params)
{
    if (loaded_) throw std::runtime_error("MonitorServer already loaded");
    settings::s_module::s_monitor params = std::get<settings::s_module::s_monitor>(module_params);
    set_last_status_(monitor_module_status::loading(params));
    try
    {
        if (params.interval <= 0)
        {
            params.interval = 1000;
        }
        boost::system::error_code ec;
        boost::asio::ip::address::from_string(params.address, ec);
        if (ec)
        {
            set_last_status_(monitor_module_status::invalid_address(params.address, ec.message()));
            logger_->error(last_status_.get_message());
            return false;
        }
        if (params.port <= 0)
        {
            set_last_status_(monitor_module_status::invalid_port(params.port));
            logger_->error(last_status_.get_message());
            return false;
        }

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

        params_ = std::move(params);
        loaded_ = true;

        set_last_status_(monitor_module_status::loaded(params));
        logger_->info(last_status_.get_message());
    }
    catch (const std::exception& e)
    {
        set_last_status_(monitor_module_status::failed_to_load(params, e.what()));
        logger_->error(e.what());
        loaded_ = false;
    }

    return loaded_;
}

bool monitor_module::reload(const module_params& moduleParams)
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

bool monitor_module::unload()
{
    if (!loaded_) throw std::runtime_error("Unavailable unload unloaded MonitorServer");
    set_last_status_(module_status::unloading());
    logger_->info(last_status_.get_message());
    std::promise<void> unloaded;
    std::future<void> future = unloaded.get_future();
    boost::asio::dispatch(strand_, [&]()
    {
        timer_.cancel();

        if (acceptor_ && acceptor_->is_open())
            acceptor_->close();

        std::for_each(sessions_.begin(), sessions_.end(), [this](const std::shared_ptr<monitor_session>& session)
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

        set_last_status_(module_status::unloaded());
        logger_->info(last_status_.get_message());
    });
    if (!strand_.running_in_this_thread())
        future.wait();

    return !loaded_;
}

bool monitor_module::loaded() const
{
    return loaded_;
}

std::string monitor_module::name() const
{
    return params_.module_name;
}

module_params monitor_module::get_params() const
{
    return params_;
}

void monitor_module::on_message(std::shared_ptr<monitor_session> session, const std::string& message)
{
    // TODO
}

void monitor_module::on_close(std::shared_ptr<monitor_session> session, const std::string& reason)
{
    boost::asio::post(strand_, [this, id = session->get_id()]
    {
        sessions_.erase(
            std::remove_if(sessions_.begin(), sessions_.end(),
                           [this, id](const std::shared_ptr<monitor_session>& s)
                           {
                               if (s->get_id() == id)
                                   logger_->info("Close connection: {}", id);
                               return s->get_id() == id;
                           }),
            sessions_.end()
        );
    });
}
