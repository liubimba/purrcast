//
// Created by bimba on 11/19/25.
//

#include "pulse_audio_loopback_module.hpp"

#include <boost/beast/http/message.hpp>
#include <boost/beast/zlib/error.hpp>
#include <boost/process/error.hpp>

#include "../../services/pulse_audio_mainloop_service.hpp"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "absl/types/optional.h"


namespace pulse
{
    void audio_loopback_module::index_cb(pa_context* ctx, uint32_t idx, void* user)
    {
        if (user == nullptr) return;
        auto request = static_cast<p_request*>(user);
        request->status = static_cast<int>(idx);
        pa_threaded_mainloop_signal(request->mainloop, 0);
    }

    void audio_loopback_module::success_cb(pa_context* ctx, int success, void* user)
    {
        if (user == nullptr) return;
        auto request = static_cast<p_request*>(user);
        request->status = success == 1 ? 0 : -1;
        pa_threaded_mainloop_signal(request->mainloop, 0);
    }

    void audio_loopback_module::server_info_callback(pa_context* ctx, const pa_server_info* server, void* user)
    {
        if (user == nullptr) return;
        auto* request = static_cast<p_request*>(user);
        request->data = server;
        request->status = 0;
        pa_threaded_mainloop_signal(request->mainloop, 1);
    }

    void audio_loopback_module::sink_info_callback(pa_context* ctx, const pa_sink_info* sink, int eol, void* user)
    {
        if (user == nullptr || sink == nullptr || eol != 0)
            return;
        auto request = static_cast<p_request*>(user);
        request->data = sink;
        request->status = 0;
        pa_threaded_mainloop_signal(request->mainloop, 1);
    }

    void audio_loopback_module::source_info_callback(pa_context* ctx, const pa_source_info* source, int eol, void* user)
    {
        if (user == nullptr || source == nullptr || eol != 0)
            return;
        auto request = static_cast<p_request*>(user);
        request->data = source;
        request->status = 0;
        pa_threaded_mainloop_signal(request->mainloop, 1);
    }

    audio_loopback_module::audio_loopback_module(const services* services, const std::string& id):
        services_(services), context_(nullptr)
    {
        logger_ = services_->get<logger_factory>()->create(absl::StrFormat("%s-loopback", id));
    }

    audio_loopback_module::~audio_loopback_module()
    {
        if (services_)
        {
            unload_safely_();

            if (services_->has<logger_factory>())
            {
                services_->get<logger_factory>()->drop(logger_);
            }
        }
    }

    bool audio_loopback_module::load(const module_params& moduleParams)
    {
        if (!std::holds_alternative<settings::s_module::s_loopback>(moduleParams))
            throw std::invalid_argument("invalid type of passed params to pulse::AudioLoopbackModule");
        if (loaded())
            throw std::runtime_error("pulse::AudioLoopbackModule already loaded");
        if (services_ == nullptr)
            throw std::runtime_error("pulse::AudioLoopbackModule services not loaded");
        if (!services_->has<mainloop_service>())
            throw std::runtime_error("pulse::AudioLoopbackModule passed services does not contain MainloopService");
        settings::s_module::s_loopback params = std::get<settings::s_module::s_loopback>(moduleParams);
        std::unique_lock lock(mutex_);
        if (!connect_()) return false;
        if (!resolve_server_info_()) return false;
        if (server_is_loopback()) return false;
        if (!load_sink_module_(params)) return false;
        if (params.set_as_default)
        {
            if (!set_as_default_output_(params)) return false;
            if (!waitUntilSetAsApply_(params, 3, absl::Seconds(1))) return false;
        }
        sink_info loopback_info = get_sink_info_by_name(params.loopback_sink_name);
        origin_device_ = get_audio_device_by_name_(server_.default_sink_name, true);
        loopback_device_ = get_audio_device_by_name_(params.loopback_sink_name, true);
        loopback_monitor_device_ = get_audio_device_by_name_(loopback_info.monitor_source_name, false);
        params_ = params;
        loaded_ = true;
        set_last_status_(loopback_module_status::running(origin_device_.name, loopback_device_.name, loopback_monitor_device_.name));
        logger_->info(last_status_.get_message());
        return true;
    }

    bool audio_loopback_module::reload(const module_params& moduleParams)
    {
        logger_->info("Request to reload");
        if (!std::holds_alternative<settings::s_module::s_loopback>(moduleParams))
            throw std::invalid_argument("invalid type of passed params to pulse::AudioLoopbackModule");
        if (!loaded())
            throw std::runtime_error("pulse::AudioLoopbackModule is not loaded to reload");
        if (services_ == nullptr)
            throw std::runtime_error("pulse::AudioLoopbackModule services not loaded");
        if (!services_->has<mainloop_service>())
            throw std::runtime_error("pulse::AudioLoopbackModule passed services does not contain MainloopService");
        settings::s_module::s_loopback params = std::get<settings::s_module::s_loopback>(moduleParams);
        if (params_ == params)
        {
            logger_->error("Reload with same parameters is pointless. Do nothing!");
            return false;
        }
        logger_->info("Unload before load again");
        unload_safely_();
        return load(moduleParams);
    }

    bool audio_loopback_module::unload()
    {
        if (!loaded())
            throw std::runtime_error("pulse::AudioLoopbackModule is not loaded to unload");
        if (services_ == nullptr)
            throw std::runtime_error("pulse::AudioLoopbackModule services not loaded");
        if (!services_->has<mainloop_service>())
            throw std::runtime_error("pulse::AudioLoopbackModule passed services does not contain MainloopService");
        unload_safely_();
        return true;
    }

    bool audio_loopback_module::loaded() const
    {
        return loaded_;
    }

    audio_device audio_loopback_module::get_audio_device_by_name_(const std::string& name, bool is_sink) const
    {
        audio_device found_device = NO_AUDIO_DEVICE;
        if (is_sink)
        {
            sink_info sink = get_sink_info_by_name(name);
            if (sink != NO_SINK_INFO)
            {
                found_device.name = sink.name;
                found_device.description = sink.description;
            }
        }
        else
        {
            source_info source = get_source_info_by_name(name);
            if (source != NO_SOURCE_INFO)
            {
                found_device.name = source.name;
                found_device.description = source.description;
            }
        }
        return found_device;
    }

    sink_info audio_loopback_module::get_sink_info_by_name(const std::string& name) const
    {
        sink_info found_sink = NO_SINK_INFO;
        pa_threaded_mainloop* mainloop = services_->get<mainloop_service>()->get();
        pa_threaded_mainloop_lock(mainloop);
        p_request req = {};
        req.mainloop = mainloop;
        pa_operation* operation = pa_context_get_sink_info_by_name(context_, name.c_str(), sink_info_callback, &req);
        if (!operation)
        {
            logger_->error("Failed to resolve sink information by name: {}. Error: failed to allocate operation", name);
            return found_sink;
        }
        while (req.data == nullptr) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(operation) != PA_OPERATION_CANCELLED)
        {
            const auto* sink = static_cast<const pa_sink_info*>(req.data);
            found_sink.name = sink->name;
            found_sink.description = sink->description;
            found_sink.monitor_source_name = sink->monitor_source_name;
        }
        else
            logger_->error("Failed to resolve sink information by name: {}. Error: operation has been cancelled", name);
        pa_operation_unref(operation);
        pa_threaded_mainloop_accept(mainloop);
        pa_threaded_mainloop_unlock(mainloop);
        return found_sink;
    }

    source_info audio_loopback_module::get_source_info_by_name(const std::string& name) const
    {
        source_info found_source = NO_SOURCE_INFO;
        pa_threaded_mainloop* mainloop = services_->get<mainloop_service>()->get();
        pa_threaded_mainloop_lock(mainloop);
        p_request req = {};
        req.mainloop = mainloop;
        pa_operation* operation = pa_context_get_source_info_by_name(context_, name.c_str(), source_info_callback, &req);
        if (!operation)
        {
            logger_->error("Failed to resolve source information by name: {}. Error: failed to allocate operation",
                           name);
            return found_source;
        }
        while (req.data == nullptr) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(operation) != PA_OPERATION_CANCELLED)
        {
            auto source = static_cast<const pa_source_info*>(req.data);
            found_source.name = source->name;
            found_source.description = source->description;
        }
        else
            logger_->error("Failed to resolve source information by name: {}. Error: operation has been cancelled",
                           name);
        pa_operation_unref(operation);
        pa_threaded_mainloop_accept(mainloop);
        pa_threaded_mainloop_unlock(mainloop);
        return found_source;
    }

    std::string audio_loopback_module::name() const
    {
        return "pulse-loopback";
    }

    module_params audio_loopback_module::get_params() const
    {
        if (!loaded()) throw std::runtime_error("AudioLoopbackModule is not loaded");
        return params_;
    }

    bool audio_loopback_module::connect_()
    {
        if (context_ == nullptr)
            context_ = services_->get<mainloop_service>()->issue("AudioLoopbackModule");

        if (pa_context_get_state(context_) == PA_CONTEXT_UNCONNECTED)
        {
            set_last_status_(loopback_module_status::context_connecting());
            logger_->info(last_status_.get_message());
            int error = pa_context_connect(context_, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
            if (error < 0) set_last_status_(loopback_module_status::failed_connect_context(pa_strerror(error)));
        }

        while (pa_context_get_state(context_) == PA_CONTEXT_CONNECTING ||
            pa_context_get_state(context_) == PA_CONTEXT_AUTHORIZING ||
            pa_context_get_state(context_) == PA_CONTEXT_SETTING_NAME)
        {
            set_last_status_(loopback_module_status::context_configuring(PA_CONTEXT_STATE_AS_STRING(pa_context_get_state(context_))));
            logger_->info(last_status_.get_message());
            absl::SleepFor(absl::Milliseconds(100));
        }

        if (pa_context_get_state(context_) == PA_CONTEXT_READY)
        {
            set_last_status_(loopback_module_status::context_connected());
            logger_->info(last_status_.get_message());
            return true;
        }

        logger_->error(last_status_.get_message());
        services_->get<mainloop_service>()->unref(context_);
        context_ = nullptr;
        return false;
    }

    bool audio_loopback_module::resolve_server_info_()
    {
        logger_->info("Resolve current server information");
        pa_threaded_mainloop* mainloop = services_->get<mainloop_service>()->get();
        p_request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        pa_operation* operation = pa_context_get_server_info(context_, server_info_callback, &request);
        if (!operation)
        {
            logger_->error("Failed to form operation to get server information");
            pa_threaded_mainloop_unlock(mainloop);
            return false;
        }
        logger_->info("Wait until operation of getting server information succeed");
        while (request.data == nullptr)
        {
            pa_threaded_mainloop_wait(mainloop);
        }
        if (pa_operation_get_state(operation) != PA_OPERATION_CANCELLED)
        {
            const auto* serverInfo = static_cast<const pa_server_info*>(request.data);
            server_ = server_info{
                .userName = serverInfo->user_name ? serverInfo->user_name : "",
                .hostName = serverInfo->host_name ? serverInfo->host_name : "",
                .serverVersion = serverInfo->server_version ? serverInfo->server_version : "",
                .serverName = serverInfo->server_name ? serverInfo->server_name : "",
                .default_sink_name = serverInfo->default_sink_name ? serverInfo->default_sink_name : "",
                .defaultSourceName = serverInfo->default_source_name ? serverInfo->default_source_name : ""
            };
            request.status = 0;
        }
        else
        {
            logger_->error("The operation of getting server information was unexpectedly interrupted");
            request.status = -1;
        }
        pa_operation_unref(operation);
        pa_threaded_mainloop_accept(mainloop);
        pa_threaded_mainloop_unlock(mainloop);
        return request.status == 0;
    }

    bool audio_loopback_module::server_is_loopback()
    {
        bool is_loopback = false;
        set_last_status_(loopback_module_status::resolving_current_type_of_device());
        logger_->info(last_status_.get_message());
        pa_threaded_mainloop* mainloop = services_->get<mainloop_service>()->get();
        p_request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        pa_operation* operation = pa_context_get_sink_info_by_name(context_, server_.default_sink_name.c_str(),
                                                                   sink_info_callback, &request);
        if (!operation)
        {
            set_last_status_(loopback_module_status::failed_to_create_operation("resolve type of audio device"));
            logger_->error(last_status_.get_message());
            pa_threaded_mainloop_unlock(mainloop);
            return false;
        }
        while (request.data == nullptr) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(operation) != PA_OPERATION_CANCELLED)
        {
            const auto* sinkInfo = static_cast<const pa_sink_info*>(request.data);
            const char* data;
            size_t size;
            pa_proplist_get(sinkInfo->proplist, "factory.name", reinterpret_cast<const void**>(&data), &size);
            if (std::strstr(data, "null-audio-sink") != nullptr)
            {
                set_last_status_(loopback_module_status::unavailable_origin_device());
                logger_->error(last_status_.get_message());
                is_loopback = true;
            }
        }
        else
        {
            set_last_status_(loopback_module_status::failed_to_create_operation("resolve type of audio device"));
            logger_->error(last_status_.get_message());
        }
        pa_operation_unref(operation);
        pa_threaded_mainloop_accept(mainloop);
        pa_threaded_mainloop_unlock(mainloop);
        return is_loopback;
    }

    bool audio_loopback_module::load_sink_module_(const settings::s_module::s_loopback& params)
    {
        set_last_status_(loopback_module_status::load_sink_module());
        logger_->info(last_status_.get_message());
        pa_threaded_mainloop* mainloop = services_->get<mainloop_service>()->get();
        p_request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        pa_operation* operation = pa_context_load_module(
            context_, "module-null-sink",
            absl::StrFormat("sink_name=%s sink_properties=device.description=%s",
                            params.loopback_sink_name, params.loopback_sink_name).c_str(),
            index_cb, &request);
        if (!operation)
        {
            set_last_status_(loopback_module_status::failed_to_create_operation("load sink module"));
            logger_->error(last_status_.get_message());
            pa_threaded_mainloop_unlock(mainloop);
            return false;
        }
        logger_->info("Wait until operation to load sink module succeed");
        while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(operation) == PA_OPERATION_DONE)
        {
            if (request.status > 0)
            {
                set_last_status_(loopback_module_status::loaded_sink_module());
                logger_->info(last_status_.get_message());
                null_sink_module_index_ = request.status;
            }
            else
            {
                set_last_status_(loopback_module_status::failed_to_load_sink_module());
                logger_->error(last_status_.get_message());
            }
        }
        else
        {
            set_last_status_(loopback_module_status::operation_interrupted("load sink module"));
            logger_->error(last_status_.get_message());
        }
        pa_operation_unref(operation);
        pa_threaded_mainloop_unlock(mainloop);
        return null_sink_module_index_ > 0;
    }

    bool audio_loopback_module::set_as_default_output_(const settings::s_module::s_loopback& params)
    {
        set_last_status_(loopback_module_status::set_as_default_output());
        logger_->info(last_status_.get_message());
        pa_threaded_mainloop* mainloop = services_->get<mainloop_service>()->get();
        p_request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        pa_operation* operation = pa_context_set_default_sink(context_, params.loopback_sink_name.c_str(), success_cb,
                                                              &request);
        if (!operation)
        {
            set_last_status_(loopback_module_status::failed_to_create_operation("set as default output"));
            logger_->error(last_status_.get_message());
            pa_threaded_mainloop_unlock(mainloop);
            return false;
        }
        logger_->info("Wait until operation to set as default output module null sink succeed");
        while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(operation) == PA_OPERATION_DONE)
        {
            if (request.status == 0)
            {
                set_last_status_(loopback_module_status::set_up_as_default_output());
                logger_->info(last_status_.get_message());
            }
            else
            {
                set_last_status_(loopback_module_status::failed_to_set_as_default_output(request.status));
                logger_->error(last_status_.get_message());
            }
        }
        else
        {
            set_last_status_(loopback_module_status::failed_to_create_operation("set as default output"));
            logger_->error(last_status_.get_message());
        }
        pa_operation_unref(operation);
        pa_threaded_mainloop_unlock(mainloop);
        return request.status == 0;
    }

    bool audio_loopback_module::waitUntilSetAsApply_(const settings::s_module::s_loopback& params, uint32_t retries,
                                                     absl::Duration timeout)
    {
        logger_->info("Waiting until set_default_sink apply");
        pa_threaded_mainloop* mainloop = services_->get<mainloop_service>()->get();
        p_request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        for (uint32_t i = 0; i < retries && request.data == nullptr; i++)
        {
            pa_operation* operation = pa_context_get_server_info(context_, server_info_callback, &request);
            if (!operation)
            {
                logger_->error("Failed to form operation to get server information");
                continue;
            }
            logger_->info("Wait until operation  to get server information succeed");
            while (request.data == nullptr) pa_threaded_mainloop_wait(mainloop);
            if (pa_operation_get_state(operation) == PA_OPERATION_CANCELED) logger_->error("Operation canceled");
            else
            {
                auto serverInfo = static_cast<const pa_server_info*>(request.data);
                if (params.loopback_sink_name == serverInfo->default_sink_name)
                {
                    logger_->info("Server successfully installed as virtual");
                    request.status = 0;
                }
                else
                {
                    logger_->error("Server still different. Expected: {}, actual: {}", params.loopback_sink_name,
                                   serverInfo->default_sink_name);
                    request.data = nullptr;
                    request.status = -1;
                }
            }
            pa_operation_unref(operation);
            pa_threaded_mainloop_accept(mainloop);
            if (request.data == nullptr)
            {
                logger_->info("Sleep before check server information again");
                absl::SleepFor(timeout);
            }
        }
        pa_threaded_mainloop_unlock(mainloop);
        return request.status == 0;
    }

    void audio_loopback_module::unload_safely_()
    {
        if (context_ && pa_context_get_state(context_) == PA_CONTEXT_READY)
        {
            if (null_sink_module_index_ > 0)
            {
                logger_->info("Unload module: {}", null_sink_module_index_);
                pa_threaded_mainloop* mainloop = services_->get<mainloop_service>()->get();
                p_request request{};
                request.data = nullptr;
                request.mainloop = mainloop;
                pa_threaded_mainloop_lock(mainloop);
                pa_operation* operation = pa_context_unload_module(context_, null_sink_module_index_, success_cb, &request);
                if (!operation)
                {
                    logger_->error("Failed to form operation unload module null sink");
                }
                else
                {
                    logger_->info("Wait until operation to unload module null sink succeed");
                    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
                        pa_threaded_mainloop_wait(
                            mainloop);
                    if (pa_operation_get_state(operation) == PA_OPERATION_CANCELED)
                        logger_->
                            error("Operation canceled");
                    else if (request.status == 0) logger_->info("Operation completed successfully");
                    else logger_->info("Operation failed with status: {}", request.status);
                }
                pa_threaded_mainloop_unlock(mainloop);
            }
            services_->get<mainloop_service>()->unref(context_);
        }
        loaded_ = false;
        context_ = nullptr;
        null_sink_module_index_ = -1;
    }

    audio_device audio_loopback_module::get_origin_device()
    {
        return origin_device_;
    }

    audio_device audio_loopback_module::get_loopback_device()
    {
        return loopback_device_;
    }

    audio_device audio_loopback_module::get_loopback_monitor_device()
    {
        return loopback_monitor_device_;
    }
}
