//
// Created by bimba on 11/16/25.
//

#ifndef SNAPCLIENTMODULE_HPP
#define SNAPCLIENTMODULE_HPP

#include "../IModule.hpp"
#include "../../util/Process.hpp"
#include "../../settings/Settings.hpp"
#include "../../services/Services.hpp"
#include "../../logger/LoggerFactory.hpp"
#include <filesystem>

#include "absl/strings/str_format.h"
#if defined(__linux__)
#include <alsa/asoundlib.h>
#endif

struct PcmDevice
{
    std::string name;
    int index = -1;
};

namespace Alsa
{
    typedef struct PcmDevice : ::PcmDevice
    {
        std::string accessLevel;
    } PcmDevice;
}

class SnapclientModule : public IModule
{
public:
    explicit SnapclientModule(const Services* services);
    bool load(const ModuleParams& params) override;
    bool reload(const ModuleParams& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] ModuleParams get_params() const override;

private:
    static PcmDevice resolveAudioDeviceName_(int sinkIndex);

    const Services* services_;
    std::mutex mutex_;
    std::unique_ptr<Process> p_snapclientProcess;
    settings::s_module::s_snapclient params_;
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //SNAPCLIENTMODULE_HPP
