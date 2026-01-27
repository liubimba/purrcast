//
// Created by bimba on 11/16/25.
//

#ifndef SNAPCLIENTMODULE_HPP
#define SNAPCLIENTMODULE_HPP

#include "../i_module.hpp"
#include "../../util/os_process.hpp"
#include "../../settings/settings.hpp"
#include "../../services/services.hpp"
#include "../../logger/logger_factory.hpp"
#include <filesystem>

#include "absl/strings/str_format.h"
#if defined(__linux__)
#include <alsa/asoundlib.h>
#endif

struct PcmDevice
{
    std::string name;
    std::string index = "-1";
};

namespace Alsa
{
    typedef struct PcmDevice : ::PcmDevice
    {
        std::string accessLevel;
    } PcmDevice;
}

class snapclient_module_status
{
public:
    static module_status loading(const settings::s_module::s_snapclient& params)
    {
        return module_status::loading(absl::StrFormat("Loading: %s %s", params.path_to_binary, params.soundcard));
    }

    static module_status binary_file_not_found(const std::string& path)
    {
        return module_status::failed(absl::StrFormat("Binary file not exists: %s", path));
    }

    static module_status failed_to_execute_binary(const std::string& path)
    {
        return module_status::failed(absl::StrFormat("Failed to execute binary: %s", path));
    }

    static module_status loaded(const std::string& path)
    {
        return module_status::loaded(absl::StrFormat("Executed binary: %s", path));
    }

    static module_status failed_to_terminate_process(const std::string& reason)
    {
        return module_status::failed(absl::StrFormat("Failed to unload module. Error: %s", reason));
    }
};

class snapclient_module : public i_module
{
public:
    explicit snapclient_module(const services* services);
    bool load(const module_params& params) override;
    bool reload(const module_params& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] module_params get_params() const override;

private:
    const services* services_;
    std::mutex mutex_;
    std::unique_ptr<os_process> snapclient_process_;
    settings::s_module::s_snapclient params_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //SNAPCLIENTMODULE_HPP
