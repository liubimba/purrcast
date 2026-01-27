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

#include "i_audio_loopback_module.hpp"
#include "../../services/services.hpp"
#include "../../logger/logger_factory.hpp"
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

static inline std::string FAILED_CONNECT_CONTEXT(const std::string& error)
{
    return absl::StrFormat("Failed to connect context. Error: %s", error);
}


namespace pulse
{
    class loopback_module_status
    {
    public:
        static module_status context_connected()
        {
            return module_status::loading("Context connected");
        }

        static module_status context_connecting()
        {
            return module_status::loading("Context connecting");
        }

        static module_status context_configuring(const std::string& state)
        {
            return module_status::loading(absl::StrFormat("Context is configuring at the moment, state: %s", state));
        }

        static module_status failed_connect_context(const std::string& error)
        {
            return module_status::failed(absl::StrFormat("Failed to connect context. Error: %s", error.c_str()));
        }

        static module_status failed_to_create_operation(const std::string& operation)
        {
            return module_status::failed(absl::StrFormat("Failed to create operation. Operation: %s", operation.c_str()));
        }

        static module_status operation_interrupted(const std::string& operation)
        {
            return module_status::failed(absl::StrFormat("Operation has been unexpectedly interrupted. Operation: %s", operation.c_str()));
        }

        static module_status resolving_current_type_of_device()
        {
            return module_status::loading("Resolving current type of audio device");
        }

        static module_status unavailable_origin_device()
        {
            return module_status::failed("Origin device is loopback");
        }

        static module_status load_sink_module()
        {
            return module_status::loading("Load virtual audio sink module");
        }

        static module_status loaded_sink_module()
        {
            return module_status::loading("Loaded virtual audio sink module");
        }

        static module_status failed_to_load_sink_module()
        {
            return module_status::failed("Failed to load virtual audio sink module");
        }

        static module_status set_as_default_output()
        {
            return module_status::loading("Set as default output virtual audio sink module");
        }

        static module_status set_up_as_default_output()
        {
            return module_status::loading("Operation to set as default output module null sink completed successfully");
        }

        static module_status failed_to_set_as_default_output(int error_code)
        {
            return module_status::failed(absl::StrFormat("Failed to set as default output virtual audio sink module. Error code: %d", error_code));
        }

        static module_status running(const std::string& orign_name, const std::string& loopback_name, const std::string& loopback_monitor_name)
        {
            return module_status::loaded(absl::StrFormat("Loaded module with: origin_device=%s, loopback_device=%s, loopback_monitor_device=%s",
                                                         orign_name, loopback_name, loopback_monitor_name));
        }
    };

    typedef struct s_server_info
    {
        std::string userName; /**< User name of the daemon process */
        std::string hostName; /**< Host name the daemon is running on */
        std::string serverVersion; /**< Version string of the daemon */
        std::string serverName; /**< Server package name (usually "pulseaudio") */
        std::string default_sink_name; /**< Name of default sink. */
        std::string defaultSourceName; /**< Name of default source. */
    } server_info;

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

    class audio_loopback_module : public i_audio_loopback_module
    {
        static void index_cb(pa_context* ctx, uint32_t idx, void* user);
        static void success_cb(pa_context* ctx, int success, void* user);
        static void server_info_callback(pa_context* ctx, const pa_server_info* server, void* user);
        static void sink_info_callback(pa_context* ctx, const pa_sink_info* sink, int eol, void* user);
        static void source_info_callback(pa_context* ctx, const pa_source_info* source, int eol, void* user);

        [[nodiscard]] sink_info get_sink_info_by_name(const std::string& name) const;
        [[nodiscard]] source_info get_source_info_by_name(const std::string& name) const;
        [[nodiscard]] audio_device get_audio_device_by_name_(const std::string& name, bool is_sink) const;

    public:
        explicit audio_loopback_module(const services* services, const std::string& id = "pulse");
        ~audio_loopback_module() override;
        bool load(const module_params& moduleParams) override;
        bool reload(const module_params& params) override;
        bool unload() override;
        [[nodiscard]] bool loaded() const override;
        [[nodiscard]] std::string name() const override;
        [[nodiscard]] module_params get_params() const override;
        audio_device get_origin_device() override;
        audio_device get_loopback_device() override;
        audio_device get_loopback_monitor_device() override;

    private:
        bool connect_();
        bool resolve_server_info_();
        bool server_is_loopback();
        bool load_sink_module_(const settings::s_module::s_loopback& params);
        bool set_as_default_output_(const settings::s_module::s_loopback& params);
        bool waitUntilSetAsApply_(const settings::s_module::s_loopback& params, uint32_t retries,
                                  absl::Duration timeout);

        void unload_safely_();

        const services* services_;
        int null_sink_module_index_ = -1;
        bool loaded_ = false;
        pa_context* context_;
        std::mutex mutex_;
        server_info server_;
        settings::s_module::s_loopback params_;
        std::shared_ptr<spdlog::logger> logger_;

        audio_device origin_device_;
        audio_device loopback_device_;
        audio_device loopback_monitor_device_;
    };
}


#endif //PULSEAUDIOLOOPBACKMODULE_HPP
