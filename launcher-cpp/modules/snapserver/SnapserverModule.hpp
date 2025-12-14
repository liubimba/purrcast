//
// Created by bimba on 11/15/25.
//

#ifndef SNAPSERVERMODULE_HPP
#define SNAPSERVERMODULE_HPP
#include "../IModule.hpp"
#include "../../services/Services.hpp"
#include "../../settings/Settings.hpp"
#include "../../util/Process.hpp"


class SnapserverModule : public IModule
{
public:
    explicit SnapserverModule(const Services* services);
    ~SnapserverModule() override;
    bool load(const ModuleParams& moduleParams) override;
    bool reload(const ModuleParams& moduleParams) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] ModuleParams getParams() const override;

private:
    std::unique_ptr<Process> p_snapserverProcess_;
    std::shared_ptr<spdlog::logger> logger_;
    settings::s_module::s_snapserver params_;
    const Services* services_;
    std::mutex mutex_;
};


#endif //SNAPSERVERMODULE_HPP
