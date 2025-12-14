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

#define OK 0

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
        std::string defaultSinkName; /**< Name of default sink. */
        std::string defaultSourceName; /**< Name of default source. */
    } ServerInfo;

    struct Request
    {
        pa_threaded_mainloop* mainloop{};
        const void* data{};
        int status = -1;
    };

    class AudioLoopbackModule : public IAudioLoopbackModule
    {
        static void indexCb(pa_context* ctx, uint32_t idx, void* user);
        static void successCb(pa_context* ctx, int success, void* user);
        static void serverInfoCb(pa_context* ctx, const pa_server_info* server, void* user);
        static void sinkInfoCb(pa_context* ctx, const pa_sink_info* sink, int eol, void* user);
        static void sourceInfoCb(pa_context* ctx, const pa_source_info* source, int eol, void* user);

    public:
        AudioLoopbackModule(const Services* services);
        ~AudioLoopbackModule() override;
        bool load(const ModuleParams& moduleParams) override;
        bool reload(const ModuleParams& params) override;
        bool unload() override;
        bool loaded() const override;
        std::string getMonitorDescription() override;
        uint32_t getSinkIndex() override;
        std::string name() const override;
        ModuleParams getParams() const override;

    private:
        bool resolveContext_();
        bool resolveServerInfo_();
        bool resolveAlsaDeviceIndex_();
        bool loadSinkModule_(const settings::s_module::s_loopback& params);
        bool setAsDefaultOutput_(const settings::s_module::s_loopback& params);
        bool waitUntilSetAsApply_(const settings::s_module::s_loopback& params, uint32_t retries,
                                  absl::Duration timeout);

        void unloadSafely_();

    private:
        const Services* services_;
        int alsaDeviceIndex_ = -1;
        int moduleIndex_ = -1;
        bool loaded_ = false;
        pa_context* context_;
        std::mutex mutex_;
        ServerInfo serverInfo_;
        settings::s_module::s_loopback params_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}


#endif //PULSEAUDIOLOOPBACKMODULE_HPP
