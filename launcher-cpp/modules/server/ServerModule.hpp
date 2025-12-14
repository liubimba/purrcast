//
// Created by bimba on 12/8/25.
//

#ifndef SERVERMODULE_HPP
#define SERVERMODULE_HPP
#include "../IModule.hpp"
#include "../../util/Process.hpp"

class ServerModule : public IModule
{
public:
    explicit ServerModule(const Services* services);
    bool load(const ModuleParams& moduleParams) override;
    bool reload(const ModuleParams& params) override;
    bool unload() override;
    bool loaded() const override;
    std::string name() const override;
    ModuleParams getParams() const override;

private:
    std::mutex mutex_;
    const Services* p_services_;
    std::unique_ptr<Process> p_process_;
    settings::s_module::s_server params_;
    std::shared_ptr<spdlog::logger> p_logger_;
};

#endif //SERVERMODULE_HPP
