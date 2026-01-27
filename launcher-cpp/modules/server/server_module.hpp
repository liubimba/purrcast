//
// Created by bimba on 12/8/25.
//

#ifndef SERVERMODULE_HPP
#define SERVERMODULE_HPP
#include "../i_module.hpp"
#include "../../util/os_process.hpp"

class server_module_status
{
public:
    static module_status loading(const settings::s_module::s_server& params)
    {
        return module_status::loading(absl::StrFormat("Loading: %s, on port=%d, static directory=%s", params.bin, params.port, params.static_dir));
    }

    static module_status invalid_port(const settings::s_module::s_server& params)
    {
        return module_status::failed(absl::StrFormat("Invalid port: %d", params.port));
    }

    static module_status bound_port(const settings::s_module::s_server& params)
    {
        return module_status::failed(absl::StrFormat("Bound port: %d", params.port));
    }

    static module_status loaded(const settings::s_module::s_server& params)
    {
        return module_status::loaded(absl::StrFormat("Loaded: %s, on port=%d", params.bin, params.port));
    }

    static module_status failed(const settings::s_module::s_server& params, const process_result& result)
    {
        return module_status::failed(absl::StrFormat("Failed to load: %s , on port=%d. Error: %s", params.bin, params.port, result.message));
    }

    static module_status unloaded_with_failures(const process_result& result)
    {
        return module_status::unloaded(absl::StrFormat("Unloaded with failures: %s", result.message));
    }
};

class server_module : public i_module
{
public:
    explicit server_module(const services* services);
    bool load(const module_params& module_params) override;
    bool reload(const module_params& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] module_params get_params() const override;

private:
    std::mutex mutex_;
    const services* services_;
    std::unique_ptr<os_process> server_process_;
    settings::s_module::s_server params_;
    std::shared_ptr<spdlog::logger> logger_;
};

#endif //SERVERMODULE_HPP
