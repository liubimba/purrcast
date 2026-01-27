//
// Created by bimba on 12/21/25.
//

#ifndef MONITORSERVER_HPP
#define MONITORSERVER_HPP

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include "monitor_session.hpp"
#include "../manager/module_manager.hpp"

class monitor_module_status
{
public:
    static module_status loading(const settings::s_module::s_monitor& params)
    {
        return module_status::loading(absl::StrFormat(""));
    }

    static module_status invalid_address(const std::string& address, const std::string& error)
    {
        return module_status::failed(absl::StrFormat("Invalid address: %s. Error: %s", address, error));
    }

    static module_status invalid_port(int port)
    {
        return module_status::failed(absl::StrFormat("Invalid port: %d", port));
    }

    static module_status failed_to_load(const settings::s_module::s_monitor& params, const std::string& message)
    {
        return module_status::failed(absl::StrFormat("Failed to load: %s:%d. Error: %s", params.address, params.port, message));
    }

    static module_status loaded(const settings::s_module::s_monitor& params)
    {
        return module_status::loaded(absl::StrFormat("Loaded %s:%d", params.address, params.port));
    }
};

class monitor_module : public i_module, public monitor_session_listener
{
public:
    explicit monitor_module(boost::asio::io_context& io_context, module_manager* module_manager,
                            const services* services);
    ~monitor_module() override;
    bool load(const module_params& module_params) override;
    bool reload(const module_params& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] module_params get_params() const override;

    void on_message(std::shared_ptr<monitor_session> session, const std::string& message) override;
    void on_close(std::shared_ptr<monitor_session> session, const std::string& reason) override;

private:
    void accept_();
    void notify_();
    void schedule_();

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    std::vector<std::shared_ptr<monitor_session>> sessions_;
    std::shared_ptr<spdlog::logger> logger_;
    settings::s_module::s_monitor params_;
    boost::asio::io_context& io_context_;
    boost::asio::steady_timer timer_;
    module_manager* module_manager_;
    const services* services_;
    std::atomic_bool loaded_;
    boost::mutex mutex_;
};


#endif //MONITORSERVER_HPP
