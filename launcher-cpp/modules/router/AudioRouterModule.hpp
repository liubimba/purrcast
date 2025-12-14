//
// Created by bimba on 11/19/25.
//

#ifndef AUDIOROUTERMODULE_HPP
#define AUDIOROUTERMODULE_HPP
#include "IAudioRouterModule.hpp"
#include "../../stream/IAudioSourceStream.hpp"


class AudioRouterModule : public IAudioRouterModule
{
public:
    explicit AudioRouterModule(const Services* services);
    ~AudioRouterModule() override;
    bool load(const ModuleParams& moduleParams) override;
    bool reload(const ModuleParams& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] ModuleParams getParams() const override;

private:
    const Services* services;
    settings::s_module::s_router params_;
    std::shared_ptr<spdlog::logger> logger_;
    std::unique_ptr<IAudioSinkStream> sinkStream_;
    std::unique_ptr<IAudioSourceStream> sourceStream_;
};


#endif //AUDIOROUTERMODULE_HPP
