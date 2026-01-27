//
// Created by bimba on 11/19/25.
//

#ifndef AUDIOROUTERMODULE_HPP
#define AUDIOROUTERMODULE_HPP
#include "i_audio_router_module.hpp"
#include "../../services/services.hpp"
#include "../../stream/i_audio_sink_stream.hpp"
#include "../../stream/i_audio_source_stream.hpp"

class router_module_status
{
public:
    static module_status loading(const settings::s_module::s_router& params)
    {
        return module_status::loading(absl::StrFormat("Loading: [SINK] name=%s, channels=%d, frames_per_buffer=%d, sample_rate=%d ; [SOURCE] name=%s, channels=%d, frames_per_buffer=%d, sample_rate=%d",
                                                      params.sink.name, params.sink.channels, params.sink.frames_per_buffer, params.sink.sample_rate,
                                                      params.source.name, params.source.channels, params.source.frames_per_buffer, params.source.sample_rate));
    }

    static module_status failed_to_start_source_stream(const std::string& error)
    {
        return module_status::failed(absl::StrFormat("Failed to create audio source stream. Error: %s", error));
    }

    static module_status started_source_stream()
    {
        return module_status::loading("Started source audio stream");
    }

    static module_status failed_to_start_sink_stream(const std::string& error)
    {
        return module_status::failed(absl::StrFormat("Failed to create audio sink stream. Error: %s", error));
    }

    static module_status started_sink_stream()
    {
        return module_status::loading("Started sink audio stream");
    }

    static module_status loaded(const settings::s_module::s_router& params)
    {
        return module_status::loaded(absl::StrFormat("Loaded: [SINK] name=%s, channels=%d, frames_per_buffer=%d, sample_rate=%d ; [SOURCE] name=%s, channels=%d, frames_per_buffer=%d, sample_rate=%d",
                                                     params.sink.name, params.sink.channels, params.sink.frames_per_buffer, params.sink.sample_rate,
                                                     params.source.name, params.source.channels, params.source.frames_per_buffer, params.source.sample_rate));
    }

    static module_status stopped_source_stream()
    {
        return module_status::unloading("Stopped source audio stream");
    }

    static module_status stopped_sink_stream()
    {
        return module_status::unloading("Stopped sink audio stream");
    }

    static module_status unloaded(const audio_stream_result& stopped_source, const audio_stream_result& stopped_sink)
    {
        if (stopped_sink.is_ok && stopped_source.is_ok)
        {
            return module_status::unloaded();
        }
        if (stopped_sink.is_ok)
        {
            return module_status::unloaded(absl::StrFormat("Failed to stop audio source stream. Error: %s", stopped_source.message));
        }
        if (stopped_source.is_ok)
        {
            return module_status::unloaded(absl::StrFormat("Failed to stop audio sink stream. Error: %s", stopped_sink.message));
        }
        return module_status::unloaded(absl::StrFormat("Failed to stop audio sink stream: %s. Failed to stop audio source stream: %s", stopped_sink.message, stopped_source.message));
    }
};

class audio_router_module : public i_audio_router_module
{
public:
    explicit audio_router_module(const services* services);
    ~audio_router_module() override;
    bool load(const module_params& moduleParams) override;
    bool reload(const module_params& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] module_params get_params() const override;
    [[nodiscard]] IAudioSource* get_source() const override;
    void add_sink(i_audio_sink* stream);
    void remove_sink(i_audio_sink* stream);
    [[nodiscard]] i_audio_source_stream* get_source_stream() const;

private:
    std::vector<i_audio_sink*> sinks_{};
    const services* services_;
    settings::s_module::s_router params_;
    std::shared_ptr<spdlog::logger> logger_;
    std::unique_ptr<i_audio_sink_stream> sink_stream_;
    std::unique_ptr<i_audio_source_stream> source_stream_;
    std::unique_ptr<i_audio_sink_stream> file_stream_;
};


#endif //AUDIOROUTERMODULE_HPP
