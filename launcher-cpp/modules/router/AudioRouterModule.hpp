//
// Created by bimba on 11/19/25.
//

#ifndef AUDIOROUTERMODULE_HPP
#define AUDIOROUTERMODULE_HPP
#include "IAudioRouterModule.hpp"
#include "../../stream/IAudioSource.hpp"
#include "../../services/Services.hpp"
#include "../../stream/IAudioSinkStream.hpp"
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
    [[nodiscard]] ModuleParams get_params() const override;

    [[nodiscard]] IAudioSource* get_source() const override;

    void add_sink(IAudioSink* stream)
    {
        sinks_.emplace_back(stream);
    }

    void remove_sink(IAudioSink* stream)
    {
        sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), stream), sinks_.end());
    }

    IAudioSourceStream* get_source_stream() const
    {
        return source_stream_.get();
    }

private:
    std::vector<IAudioSink*> sinks_{};
    const Services* services;
    settings::s_module::s_router params_;
    std::shared_ptr<spdlog::logger> logger_;
    std::unique_ptr<IAudioSinkStream> sink_stream_;
    std::unique_ptr<IAudioSourceStream> source_stream_;
    std::unique_ptr<IAudioSinkStream> file_stream_;
};


#endif //AUDIOROUTERMODULE_HPP
