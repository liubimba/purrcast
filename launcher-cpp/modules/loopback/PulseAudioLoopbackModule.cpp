//
// Created by bimba on 11/19/25.
//

#include "PulseAudioLoopbackModule.hpp"

#include <boost/beast/http/message.hpp>
#include <boost/beast/zlib/error.hpp>
#include <boost/process/error.hpp>

#include "../../services/PulseMainloopService.hpp"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "absl/types/optional.h"


namespace pulse
{
    void AudioLoopbackModule::indexCb(pa_context* ctx, uint32_t idx, void* user)
    {
        if (user == nullptr) return;
        auto request = static_cast<Request*>(user);
        request->status = static_cast<int>(idx);
        pa_threaded_mainloop_signal(request->mainloop, 0);
    }

    void AudioLoopbackModule::successCb(pa_context* ctx, int success, void* user)
    {
        if (user == nullptr) return;
        auto request = static_cast<Request*>(user);
        request->status = success == 1 ? OK : -1;
        pa_threaded_mainloop_signal(request->mainloop, 0);
    }

    void AudioLoopbackModule::serverInfoCb(pa_context* ctx, const pa_server_info* server, void* user)
    {
        if (user == nullptr) return;
        auto* request = static_cast<Request*>(user);
        request->data = server;
        request->status = OK;
        pa_threaded_mainloop_signal(request->mainloop, 1);
    }

    void AudioLoopbackModule::sinkInfoCb(pa_context* ctx, const pa_sink_info* sink, int eol, void* user)
    {
        if (user == nullptr || sink == nullptr || eol != 0)
            return;
        auto request = static_cast<Request*>(user);
        request->data = sink;
        request->status = OK;
        pa_threaded_mainloop_signal(request->mainloop, 1);
    }

    void AudioLoopbackModule::sourceInfoCb(pa_context* ctx, const pa_source_info* source, int eol, void* user)
    {
        if (user == nullptr || source == nullptr || eol != 0)
            return;
        auto request = static_cast<Request*>(user);
        request->data = source;
        request->status = OK;
        pa_threaded_mainloop_signal(request->mainloop, 1);
    }

    AudioLoopbackModule::AudioLoopbackModule(const Services* services):
        services_(services), context_(nullptr)
    {
        logger_ = services_->get<LoggerFactory>()->create("PulseAudioLoopbackModule");
    }

    AudioLoopbackModule::~AudioLoopbackModule()
    {
        if (services_)
        {
            unloadSafely_();

            if (services_->has<LoggerFactory>())
            {
                services_->get<LoggerFactory>()->drop(logger_);
            }
        }
    }

    bool AudioLoopbackModule::load(const ModuleParams& moduleParams)
    {
        if (!std::holds_alternative<settings::s_module::s_loopback>(moduleParams))
            throw std::invalid_argument("invalid type of passed params to pulse::AudioLoopbackModule");
        if (loaded())
            throw std::runtime_error("pulse::AudioLoopbackModule already loaded");
        if (services_ == nullptr)
            throw std::runtime_error("pulse::AudioLoopbackModule services not loaded");
        if (!services_->has<MainloopService>())
            throw std::runtime_error("pulse::AudioLoopbackModule passed services does not contain MainloopService");
        settings::s_module::s_loopback params = std::get<settings::s_module::s_loopback>(moduleParams);
        std::unique_lock lock(mutex_);
        if (!resolveContext_()) return false;
        if (!resolveServerInfo_()) return false;
        if (!resolveAlsaDeviceIndex_()) return false;
        if (!loadSinkModule_(params)) return false;
        if (!setAsDefaultOutput_(params)) return false;
        if (!waitUntilSetAsApply_(params, 3, absl::Seconds(1))) return false;
        params_ = params;
        loaded_ = true;
        return true;
    }

    bool AudioLoopbackModule::reload(const ModuleParams& moduleParams)
    {
        logger_->info("Request to reload");
        if (!std::holds_alternative<settings::s_module::s_loopback>(moduleParams))
            throw std::invalid_argument("invalid type of passed params to pulse::AudioLoopbackModule");
        if (!loaded())
            throw std::runtime_error("pulse::AudioLoopbackModule is not loaded to reload");
        if (services_ == nullptr)
            throw std::runtime_error("pulse::AudioLoopbackModule services not loaded");
        if (!services_->has<MainloopService>())
            throw std::runtime_error("pulse::AudioLoopbackModule passed services does not contain MainloopService");
        settings::s_module::s_loopback params = std::get<settings::s_module::s_loopback>(moduleParams);
        if (params_ == params)
        {
            logger_->error("Reload with same parameters is pointless. Do nothing!");
            return false;
        }
        logger_->info("Unload before load again");
        unloadSafely_();
        return load(moduleParams);
    }

    bool AudioLoopbackModule::unload()
    {
        if (!loaded())
            throw std::runtime_error("pulse::AudioLoopbackModule is not loaded to unload");
        if (services_ == nullptr)
            throw std::runtime_error("pulse::AudioLoopbackModule services not loaded");
        if (!services_->has<MainloopService>())
            throw std::runtime_error("pulse::AudioLoopbackModule passed services does not contain MainloopService");
        unloadSafely_();
        return !loaded_;
    }

    bool AudioLoopbackModule::loaded() const
    {
        return loaded_;
    }

    std::string AudioLoopbackModule::getMonitorDescription()
    {
        if (!loaded())
            throw std::runtime_error("pulse::AudioLoopbackModule is not loaded");
        pa_threaded_mainloop* mainloop = services_->get<MainloopService>()->get();
        pa_threaded_mainloop_lock(mainloop);
        Request sinkRequest{};
        std::memset(&sinkRequest, 0, sizeof(sinkRequest));
        sinkRequest.mainloop = mainloop;
        pa_operation* sinkOperation = pa_context_get_sink_info_by_name(context_, params_.virtualSinkName.c_str(),
                                                                       sinkInfoCb, &sinkRequest);
        if (!sinkOperation)
        {
            pa_threaded_mainloop_unlock(mainloop);
            throw std::runtime_error("Failed to get sink information of server");
        }
        while (sinkRequest.data == nullptr) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(sinkOperation) == PA_OPERATION_CANCELLED)
        {
            pa_threaded_mainloop_unlock(mainloop);
            pa_operation_unref(sinkOperation);
            throw std::runtime_error("Operation to get sink information cancelled");
        }
        auto sinkInfo = static_cast<const pa_sink_info*>(sinkRequest.data);
        Request sourceRequest{};
        std::memset(&sourceRequest, 0, sizeof(sourceRequest));
        sourceRequest.mainloop = mainloop;
        std::string sourceMonitorName{sinkInfo->monitor_source_name};
        pa_operation* sourceOperation = pa_context_get_source_info_by_name(
            context_, sourceMonitorName.c_str(), sourceInfoCb, &sourceRequest);
        pa_threaded_mainloop_accept(mainloop);
        if (!sourceOperation)
        {
            pa_threaded_mainloop_unlock(mainloop);
            throw std::runtime_error("Failed to get source information");
        }
        while (sourceRequest.data == nullptr) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(sourceOperation) == PA_OPERATION_CANCELLED)
        {
            pa_operation_unref(sourceOperation);
            pa_threaded_mainloop_unlock(mainloop);
            throw std::runtime_error("Operation to get source information cancelled");
        }
        auto sourceInfo = static_cast<const pa_source_info*>(sourceRequest.data);
        std::string sourceDescription{sourceInfo->description};
        pa_operation_unref(sourceOperation);
        pa_threaded_mainloop_accept(mainloop);
        pa_threaded_mainloop_unlock(mainloop);
        return sourceDescription;
    }

    uint32_t AudioLoopbackModule::getSinkIndex()
    {
        return alsaDeviceIndex_;
    }

    std::string AudioLoopbackModule::name() const
    {
        return "pulse-loopback";
    }

    ModuleParams AudioLoopbackModule::getParams() const
    {
        if (!loaded()) throw std::runtime_error("AudioLoopbackModule is not loaded");
        return params_;
    }

    bool AudioLoopbackModule::resolveContext_()
    {
        if (context_ == nullptr)
            context_ = services_->get<MainloopService>()->issue("AudioLoopbackModule");

        if (pa_context_get_state(context_) == PA_CONTEXT_UNCONNECTED)
        {
            int error = pa_context_connect(context_, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
            if (error < 0)
            {
                logger_->error("Failed to connect context. Error: {}", pa_strerror(error));
                services_->get<MainloopService>()->unref(context_);
                return false;
            }
        }

        while (pa_context_get_state(context_) == PA_CONTEXT_CONNECTING ||
            pa_context_get_state(context_) == PA_CONTEXT_AUTHORIZING ||
            pa_context_get_state(context_) == PA_CONTEXT_SETTING_NAME)
        {
            logger_->info("Context is configuring at the moment, state: {}",
                          PA_CONTEXT_STATE_AS_STRING(pa_context_get_state(context_)));
            absl::SleepFor(absl::Milliseconds(100));
        }

        if (pa_context_get_state(context_) == PA_CONTEXT_FAILED)
        {
            logger_->error("Some error occurred during connect context");
            services_->get<MainloopService>()->unref(context_);
            context_ = nullptr;
            return false;
        }

        if (pa_context_get_state(context_) == PA_CONTEXT_TERMINATED)
        {
            logger_->error("Context terminated");
            services_->get<MainloopService>()->unref(context_);
            context_ = nullptr;
            return false;
        }

        if (pa_context_get_state(context_) != PA_CONTEXT_READY)
        {
            logger_->error("Context not ready, undefined context state");
            services_->get<MainloopService>()->unref(context_);
            context_ = nullptr;
            return false;
        }

        return true;
    }

    bool AudioLoopbackModule::resolveServerInfo_()
    {
        logger_->info("Resolve current server information");
        pa_threaded_mainloop* mainloop = services_->get<MainloopService>()->get();
        Request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        pa_operation* operation = pa_context_get_server_info(context_, serverInfoCb, &request);
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
            serverInfo_ = ServerInfo{
                .userName = serverInfo->user_name ? serverInfo->user_name : "",
                .hostName = serverInfo->host_name ? serverInfo->host_name : "",
                .serverVersion = serverInfo->server_version ? serverInfo->server_version : "",
                .serverName = serverInfo->server_name ? serverInfo->server_name : "",
                .defaultSinkName = serverInfo->default_sink_name ? serverInfo->default_sink_name : "",
                .defaultSourceName = serverInfo->default_source_name ? serverInfo->default_source_name : ""
            };
            request.status = OK;
        }
        else
        {
            logger_->error("The operation of getting server information was unexpectedly interrupted");
            request.status = -1;
        }
        pa_operation_unref(operation);
        pa_threaded_mainloop_accept(mainloop);
        pa_threaded_mainloop_unlock(mainloop);
        return request.status == OK;
    }

    bool AudioLoopbackModule::resolveAlsaDeviceIndex_()
    {
        logger_->info("Resolve alsa.device index of current sink");
        pa_threaded_mainloop* mainloop = services_->get<MainloopService>()->get();
        Request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        pa_operation* operation = pa_context_get_sink_info_by_name(context_, serverInfo_.defaultSinkName.c_str(),
                                                                   sinkInfoCb, &request);
        if (!operation)
        {
            logger_->error("Failed to form operation to resolve alsa.device index");
            pa_threaded_mainloop_unlock(mainloop);
            return false;
        }
        logger_->info("Wait until operation to resolve alsa.device index succeed");
        while (request.data == nullptr)
        {
            pa_threaded_mainloop_wait(mainloop);
        }
        if (pa_operation_get_state(operation) != PA_OPERATION_CANCELLED)
        {
            const auto* sinkInfo = static_cast<const pa_sink_info*>(request.data);
            const char* data;
            size_t size;
            pa_proplist_get(sinkInfo->proplist, "factory.name", reinterpret_cast<const void**>(&data), &size);
            if (std::strstr(data, "null-audio-sink") != nullptr)
            {
                logger_->error("Current audio server is loopback already!");
            }
            else
            {
                int error = pa_proplist_contains(sinkInfo->proplist, "alsa.device");
                if (error < 0)
                {
                    logger_->error("Failed to find alsa.device on current server information. Error: {}",
                                   pa_strerror(error));
                }
                else
                {
                    pa_proplist_get(sinkInfo->proplist, "alsa.device", reinterpret_cast<const void**>(&data), &size);
                    alsaDeviceIndex_ = std::stoi(data);
                    logger_->info("Resolve alsa.device={} index", alsaDeviceIndex_);
                }
            }
        }
        else
        {
            logger_->error("The operation was unexpectedly interrupted");
        }
        pa_operation_unref(operation);
        pa_threaded_mainloop_accept(mainloop);
        pa_threaded_mainloop_unlock(mainloop);
        return alsaDeviceIndex_ != -1;
    }

    bool AudioLoopbackModule::loadSinkModule_(const settings::s_module::s_loopback& params)
    {
        logger_->info("Load sink module");
        pa_threaded_mainloop* mainloop = services_->get<MainloopService>()->get();
        Request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        pa_operation* operation = pa_context_load_module(
            context_, "module-null-sink",
            absl::StrFormat("sink_name=%s sink_properties=device.description=%s",
                            params.virtualSinkName, params.virtualSinkName).c_str(),
            indexCb, &request);
        if (!operation)
        {
            logger_->error("Failed to form operation to load sink module");
            pa_threaded_mainloop_unlock(mainloop);
            return false;
        }
        logger_->info("Wait until operation to load sink module succeed");
        while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(operation) == PA_OPERATION_DONE)
        {
            if (request.status > 0)
            {
                logger_->info("Operation to load sink module completed successfully");
                moduleIndex_ = request.status;
            }
            else logger_->info("Operation to load sink module failed with status: {}", request.status);
        }
        else logger_->error("The operation to load sink module was unexpectedly interrupted");
        pa_operation_unref(operation);
        pa_threaded_mainloop_unlock(mainloop);
        return moduleIndex_ > 0;
    }

    bool AudioLoopbackModule::setAsDefaultOutput_(const settings::s_module::s_loopback& params)
    {
        logger_->info("Set as default output module null sink");
        pa_threaded_mainloop* mainloop = services_->get<MainloopService>()->get();
        Request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        pa_operation* operation = pa_context_set_default_sink(context_, params.virtualSinkName.c_str(), successCb,
                                                              &request);
        if (!operation)
        {
            logger_->error("Failed to form operation to set as default output module null sink");
            pa_threaded_mainloop_unlock(mainloop);
            return false;
        }
        logger_->info("Wait until operation to set as default output module null sink succeed");
        while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING) pa_threaded_mainloop_wait(mainloop);
        if (pa_operation_get_state(operation) == PA_OPERATION_DONE)
        {
            if (request.status == OK)
                logger_->info(
                    "Operation to set as default output module null sink completed successfully");
            else
                logger_->info("Operation to set as default output module null sink failed with status: {}",
                              request.status);
        }
        else
            logger_->error("The operation to set as default output module null sink was unexpectedly interrupted");
        pa_operation_unref(operation);
        pa_threaded_mainloop_unlock(mainloop);
        return request.status == OK;
    }

    bool AudioLoopbackModule::waitUntilSetAsApply_(const settings::s_module::s_loopback& params, uint32_t retries,
                                                   absl::Duration timeout)
    {
        logger_->info("Waiting until set_default_sink apply");
        pa_threaded_mainloop* mainloop = services_->get<MainloopService>()->get();
        Request request{};
        request.data = nullptr;
        request.mainloop = mainloop;
        pa_threaded_mainloop_lock(mainloop);
        for (uint32_t i = 0; i < retries && request.data == OK; i++)
        {
            pa_operation* operation = pa_context_get_server_info(context_, serverInfoCb, &request);
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
                if (params.virtualSinkName == serverInfo->default_sink_name)
                {
                    logger_->info("Server successfully installed as virtual");
                    request.status = OK;
                }
                else
                {
                    logger_->error("Server still different. Expected: {}, actual: {}", params.virtualSinkName,
                                   serverInfo->default_sink_name);
                    request.data = nullptr;
                    request.status = -1;
                }
            }
            pa_operation_unref(operation);
            pa_threaded_mainloop_accept(mainloop);
            logger_->info("Sleep before check server information again");
            absl::SleepFor(timeout);
        }
        pa_threaded_mainloop_unlock(mainloop);
        return request.status == OK;
    }

    void AudioLoopbackModule::unloadSafely_()
    {
        if (context_ && pa_context_get_state(context_) == PA_CONTEXT_READY)
        {
            if (moduleIndex_ > 0)
            {
                logger_->info("Unload module: {}", moduleIndex_);
                pa_threaded_mainloop* mainloop = services_->get<MainloopService>()->get();
                Request request{};
                request.data = nullptr;
                request.mainloop = mainloop;
                pa_threaded_mainloop_lock(mainloop);
                pa_operation* operation = pa_context_unload_module(context_, moduleIndex_, successCb, &request);
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
                    else if (request.status == OK) logger_->info("Operation completed successfully");
                    else logger_->info("Operation failed with status: {}", request.status);
                }
                pa_threaded_mainloop_unlock(mainloop);
            }
            services_->get<MainloopService>()->unref(context_);
        }
        loaded_ = false;
        context_ = nullptr;
        moduleIndex_ = -1;
        alsaDeviceIndex_ = -1;
    }
}
