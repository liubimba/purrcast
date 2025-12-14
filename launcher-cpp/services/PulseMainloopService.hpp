//
// Created by bimba on 11/19/25.
//

#ifndef PULSEMAINLOOPSERVICE_HPP
#define PULSEMAINLOOPSERVICE_HPP
#include <memory>

#include "Services.hpp"
#include "pulse/pulseaudio.h"
#include "spdlog/spdlog.h"

namespace pulse
{
    class MainloopService : public IService
    {
    public:
        explicit MainloopService(const Services* services);
        ~MainloopService();
        pa_threaded_mainloop* get();
        pa_context* issue(const char* id);
        void unref(pa_context* ctx);
        int getIssued();
        bool available() override;

    private:
        bool available_;
        const Services* services_;
        std::atomic<int> count_;
        pa_threaded_mainloop* mainloop_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}


#endif //PULSEMAINLOOPSERVICE_HPP
