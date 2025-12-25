//
// Created by bimba on 12/21/25.
//

#ifndef MONITORSERVER_HPP
#define MONITORSERVER_HPP

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include "MonitorSession.hpp"
#include "../manager/ModuleManager.hpp"


class MonitorModule : public IModule, public MonitorSessionListener
{
public:
    explicit MonitorModule(boost::asio::io_context& io_context, ModuleManager* module_manager,
                           const Services* services);
    ~MonitorModule() override;
    bool load(const ModuleParams& moduleParams) override;
    bool reload(const ModuleParams& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] ModuleParams get_params() const override;

    void onMessage(std::shared_ptr<MonitorSession> session, const std::string& message) override;
    void onClose(std::shared_ptr<MonitorSession> session, const std::string& reason) override;

private:
    void accept_();
    void notify_();
    void schedule_();

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    std::vector<std::shared_ptr<MonitorSession>> sessions_;
    std::shared_ptr<spdlog::logger> logger_;
    settings::s_module::s_monitor params_;
    boost::asio::io_context& io_context_;
    boost::asio::steady_timer timer_;
    ModuleManager* module_manager_;
    const Services* services_;
    std::atomic_bool loaded_;
    boost::mutex mutex_;
};


#endif //MONITORSERVER_HPP
