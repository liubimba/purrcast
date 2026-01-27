//
// Created by bimba on 11/19/25.
//

#include "pulse_audio_mainloop_service.hpp"

#include "../logger/logger_factory.hpp"

namespace pulse
{
    mainloop_service::mainloop_service(const services* services):
        services_(services), count_(0)
    {
        logger_ = services_->get<logger_factory>()->create("MainloopService");
        mainloop_ = pa_threaded_mainloop_new();
        if (int error = pa_threaded_mainloop_start(mainloop_) != 0)
        {
            logger_->error("Failed to start serving thread. Error: {}", pa_strerror(error));
            available_ = false;
        }
        else
        {
            available_ = true;
        }
    }

    mainloop_service::~mainloop_service()
    {
        logger_->info("Destruct");
        if (count_ > 0)
        {
            logger_->error("Possible leaks, not all context were unrefed");
        }
        pa_threaded_mainloop_stop(mainloop_);
        pa_threaded_mainloop_free(mainloop_);
        services_->get<logger_factory>()->drop(logger_);
    }

    pa_threaded_mainloop* mainloop_service::get()
    {
        return mainloop_;
    }

    pa_context* mainloop_service::issue(const char* id)
    {
        if (pa_context* ctx = pa_context_new(pa_threaded_mainloop_get_api(mainloop_), id))
        {
            count_.fetch_add(1);
            logger_->info("Issued new context");
            return ctx;
        }
        logger_->error("Failed to issue new context");
        return nullptr;
    }

    void mainloop_service::unref(pa_context* ctx)
    {
        if (ctx == nullptr)
        {
            return;
        }
        logger_->info("Unref context");
        count_.fetch_sub(1);
        pa_context_unref(ctx);
    }

    int mainloop_service::getIssued()
    {
        return count_.load();
    }

    bool mainloop_service::available()
    {
        return available_;
    }
}
