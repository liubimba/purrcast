//
// Created by bimba on 12/26/25.
//

#ifndef TESTCLIENTMODULE_HPP
#define TESTCLIENTMODULE_HPP

#include "../IModule.hpp"
#include "../../stream/IAudioSink.hpp"
#include <atomic>
#include <memory>
#include <boost/asio/steady_timer.hpp>
#include "../../stream/IAudioSourceStream.hpp"
#include "../loopback/AudioLoopbackModule.hpp"
#include "../snapclient/SnapclientModule.hpp"
#include "audio_inspector/IAudioInspector.hpp"
#include "audio_inspector/FingerprintAudioInspector.hpp"
#include "listener/ProxyAudioSinkListener.hpp"
#include "../../stream/PortAudioSourceStream.hpp"

class TestEnvironmentModule : public IModule
{
public:
    explicit TestEnvironmentModule(boost::asio::io_context& io_context, const Services* services,
                                   IAudioSource* source_stream);

    bool load(const ModuleParams& params) override;
    bool reload(const ModuleParams& params) override;
    bool unload() override;
    [[nodiscard]] bool loaded() const override;
    [[nodiscard]] std::string name() const override;
    [[nodiscard]] ModuleParams get_params() const override;

    ~TestEnvironmentModule() override = default;

private:
    std::unique_ptr<IAudioSourceStream> loopback_capture_;
    std::unique_ptr<IAudioInspector> audio_inspector_;
    std::unique_ptr<IAudioSink> source_listener_;
    std::unique_ptr<IAudioSink> snapcast_listener_;
    const Services* services_{};
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
