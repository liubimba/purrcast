//
// Created by bimba on 11/15/25.
//

#ifndef SNAPSERVERMODULE_HPP
#define SNAPSERVERMODULE_HPP
#include "../i_module.hpp"
#include "../../services/services.hpp"
#include "../../settings/settings.hpp"
#include "../../util/os_process.hpp"

class snapserver_module_status
{
public:
    static module_status loading(const std::string& cmd, const std::string& args)
    {
        return module_status::loading(absl::StrFormat("Loading module, execute binary: %s %s", cmd, args));
    }

    static module_status invalid_ports(const settings::s_module::s_snapserver::s_ports& ports)
    {
        return module_status::failed(absl::StrFormat("Invalid ports: http.port=%d, stream.port=%d, control.port=%d", ports.http, ports.stream, ports.control));
    }

    static module_status loaded(const std::string& cmd, const std::string& args)
    {
        return module_status::loaded(absl::StrFormat("Loaded module: %s %s", cmd, args));
    }

    static module_status failed_to_load(const std::string& error)
    {
        return module_status::failed(absl::StrFormat("Failed to execute binary. Error: %s", error));
    }

    static module_status unloaded_with_errors(const std::string& error)
    {
        return module_status::unloaded(absl::StrFormat("Unloaded with errors: %s", error));
    }
};

class snapserver_module : public i_module
{
public:
    explicit snapserver_module(const services* services);
    ~snapserver_module() override;
    bool load(const module_params& moduleParams) override;
    bool reload(const module_params& moduleParams) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] module_params get_params() const override;

private:
    std::unique_ptr<os_process> snapserver_process_;
    std::shared_ptr<spdlog::logger> logger_;
    settings::s_module::s_snapserver params_;
    const services* services_;
    std::mutex mutex_;
};


#endif //SNAPSERVERMODULE_HPP
