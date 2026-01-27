//
// Created by bimba on 12/26/25.
//

#ifndef TESTCLIENTMODULE_HPP
#define TESTCLIENTMODULE_HPP

#include "../i_module.hpp"
#include "../../stream/i_audio_sink.hpp"
#include <atomic>
#include <memory>
#include <boost/asio/steady_timer.hpp>
#include "../../stream/i_audio_source_stream.hpp"
#include "../loopback/audio_loopback_module.hpp"
#include "../snapclient/snapclient_module.hpp"
#include "audio_inspector/i_audio_inspector.hpp"
#include "audio_inspector/fingerprint_audio_inspector.hpp"
#include "listener/proxy_audio_sink_listener.hpp"
#include "../../stream/port_audio_source_stream.hpp"

class test_environment_module_status
{
public:
    static module_status loading(const settings::s_module::s_test_environment& params)
    {
        return module_status::loading(absl::StrFormat("Loading loopback=%s, snapclient binary: %s", params.loopback.loopback_sink_name, params.snapclient.path_to_binary));
    }

    static module_status failed_to_start_loopback(const audio_stream_result& result)
    {
        return module_status::failed(absl::StrFormat("Failed to start virtual audio loopback. Error: %s", result.message));
    }

    static module_status loaded(const settings::s_module::s_test_environment& params)
    {
        return module_status::loaded(absl::StrFormat("Loaded loopback=%s, snapclient binary: %s", params.loopback.loopback_sink_name, params.snapclient.path_to_binary));
    }

    static module_status unloaded(const audio_stream_result& result)
    {
        if (result.is_ok)
        {
            return module_status::unloaded();
        }
        return module_status::unloaded(absl::StrFormat("Unloaded with failures: %s", result.message));
    }
};

class test_environment : public i_module
{
public:
    explicit test_environment(boost::asio::io_context& io_context, const services* services,
                              IAudioSource* source_stream);

    bool load(const module_params& params) override;
    bool reload(const module_params& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] module_params get_params() const override;

    ~test_environment() override = default;

private:
    std::unique_ptr<i_audio_source_stream> loopback_capture_;
    std::unique_ptr<i_audio_inspector> audio_inspector_;
    std::unique_ptr<i_audio_sink> source_listener_;
    std::unique_ptr<i_audio_sink> snapcast_listener_;
    const services* services_{};
    IAudioSource* source_stream_;
    boost::asio::io_context& io_context_;

    std::atomic<uint32_t> sequence_number_{0};
    std::atomic_bool loaded_{false};
    bool log_verbose_{true};

    std::shared_ptr<spdlog::logger> logger_;
    settings::s_module::s_test_environment params_;

    void on_source_chunk_(const audio_chunk& chunk);
    void on_snapcast_chunk_(const audio_chunk& chunk);
};

#endif //TESTCLIENTMODULE_HPP
