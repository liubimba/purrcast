//
// Created by bimba on 11/19/25.
//

#ifndef PULSEAUDIOLOOPBACKMODULE_HPP
#define PULSEAUDIOLOOPBACKMODULE_HPP
#include <mutex>
#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/thread-mainloop.h>

#include <spdlog/spdlog.h>

#include "IAudioLoopbackModule.hpp"
#include "../../services/Services.hpp"
#include "../../logger/LoggerFactory.hpp"
#include "absl/time/time.h"

#define PA_CONTEXT_STATE_LIST(X) \
X(PA_CONTEXT_UNCONNECTED)       \
X(PA_CONTEXT_CONNECTING)        \
X(PA_CONTEXT_AUTHORIZING)       \
X(PA_CONTEXT_SETTING_NAME)      \
X(PA_CONTEXT_READY)             \
X(PA_CONTEXT_FAILED)            \
X(PA_CONTEXT_TERMINATED)

#define PA_CONTEXT_STATE_LIST_ITEM(x) case x: return #x;

static inline const char* PA_CONTEXT_STATE_AS_STRING(pa_context_state_t state)
{
    switch (state)
    {
    PA_CONTEXT_STATE_LIST(PA_CONTEXT_STATE_LIST_ITEM)
    default: return "PA_CONTEXT_UNKNOWN";
    }
}

namespace pulse
{
    typedef struct ServerInfo
    {
        std::string userName; /**< User name of the daemon process */
        std::string hostName; /**< Host name the daemon is running on */
        std::string serverVersion; /**< Version string of the daemon */
        std::string serverName; /**< Server package name (usually "pulseaudio") */
        std::string default_sink_name; /**< Name of default sink. */
        std::string defaultSourceName; /**< Name of default source. */
    } ServerInfo;

    typedef struct s_sink_info
    {
        std::string name;
        std::string description;
        std::string monitor_source_name;

        bool operator==(const s_sink_info& oth) const
        {
            return name == oth.name && description == oth.description;
        }

        bool operator!=(const s_sink_info& oth) const
        {
            return !(oth == *this);
        }
    } sink_info;

    typedef struct s_source_info
    {
        std::string name;
        std::string description;

        bool operator==(const s_source_info& oth) const
        {
            return name == oth.name && description == oth.description;
        }

        bool operator!=(const s_source_info& oth) const
        {
            return !(oth == *this);
        }
    } source_info;

    const sink_info NO_SINK_INFO = {.name = "NOT_FOUND", .description = "NOT_FOUND"};
    const source_info NO_SOURCE_INFO = {.name = "NOT_FOUND", .description = "NOT_FOUND"};

    struct p_request
    {
        pa_threaded_mainloop* mainloop{};
        const void* data{};
        int status = -1;
    };

    class AudioLoopbackModule : public IAudioLoopbackModule
    {
        static void indexCb(pa_context* ctx, uint32_t idx, void* user);
        static void successCb(pa_context* ctx, int success, void* user);
        static void server_info_callback(pa_context* ctx, const pa_server_info* server, void* user);
        static void sink_info_callback(pa_context* ctx, const pa_sink_info* sink, int eol, void* user);
        static void source_info_callback(pa_context* ctx, const pa_source_info* source, int eol, void* user);

        audio_device get_audio_device_by_name_(const std::string& name, bool is_sink);
        sink_info get_sink_info_by_name(const std::string& name);
        source_info get_source_info_by_name(const std::string& name);

    public:
        explicit AudioLoopbackModule(const Services* services, const std::string& id = "pulse");
        ~AudioLoopbackModule() override;
        bool load(const ModuleParams& moduleParams) override;
        bool reload(const ModuleParams& params) override;
        bool unload() override;
        [[nodiscard]] bool loaded() const override;
        std::string get_loopback_monitor_description() override;
        std::string get_origin_alsa_device() override;
        [[nodiscard]] std::string name() const override;
        [[nodiscard]] ModuleParams get_params() const override;
        std::string get_loopback_alsa_device() override;
        std::string get_loopback_description() override;

    private:
        bool connect_();
        bool resolve_server_info_();
        bool resolve_alsa_device_index_of_origin(settings::s_module::s_loopback params);
        bool load_sink_module_(const settings::s_module::s_loopback& params);
        bool set_as_default_output_(const settings::s_module::s_loopback& params);
        bool waitUntilSetAsApply_(const settings::s_module::s_loopback& params, uint32_t retries,
                                  absl::Duration timeout);

        void unloadSafely_();

    public:
        audio_device get_origin_device() override;
        audio_device get_loopback_device() override;
        audio_device get_loopback_monitor_device() override;

    private:
        const Services* services_;
        int origin_alsa_device_index_ = -1;
        int moduleIndex_ = -1;
        bool loaded_ = false;
        pa_context* context_;
        std::mutex mutex_;
        ServerInfo server_;
        settings::s_module::s_loopback params_;
        std::shared_ptr<spdlog::logger> logger_;

        audio_device origin_device_;
        audio_device loopback_device_;
        audio_device loopback_monitor_device_;
    };
}


#endif //PULSEAUDIOLOOPBACKMODULE_HPP
