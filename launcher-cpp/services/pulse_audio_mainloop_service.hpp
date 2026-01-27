//
// Created by bimba on 11/19/25.
//

#ifndef PULSEMAINLOOPSERVICE_HPP
#define PULSEMAINLOOPSERVICE_HPP
#include <memory>

#include "services.hpp"
#include "pulse/pulseaudio.h"
#include "spdlog/spdlog.h"

namespace pulse
{
    class mainloop_service : public i_service
    {
    public:
        explicit mainloop_service(const services* services);
        ~mainloop_service() override;
        pa_threaded_mainloop* get();
        pa_context* issue(const char* id);
        void unref(pa_context* ctx);
        int getIssued();
        bool available() override;

    private:
        bool available_;
        const services* services_;
        std::atomic<int> count_;
        pa_threaded_mainloop* mainloop_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}


#endif //PULSEMAINLOOPSERVICE_HPP
