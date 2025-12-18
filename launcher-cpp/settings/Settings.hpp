//
// Created by bimba on 11/15/25.
//

#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <variant>
#include <boost/chrono/ceil.hpp>

#include "../modules/ModuleDescription.hpp"
#include "absl/log/internal/config.h"
#include "absl/strings/str_format.h"
#include "spdlog/spdlog.h"


struct settings
{
    struct s_log
    {
        spdlog::level::level_enum level = spdlog::level::info;
    };

    struct s_manager
    {
        struct s_interval
        {
            boost::chrono::duration<double> healthy = boost::chrono::seconds(5);
            boost::chrono::duration<double> unhealthy = boost::chrono::seconds(1);
        };

        s_interval interval;
    };

    struct s_module
    {
        struct s_server : module_description
        {
            std::string bin;
            std::string static_dir;
            int port = -1;

            s_server()
            {
                name = "server";
            }

            s_server(const s_server& oth) = default;
            s_server& operator=(const s_server& oth) = default;

            bool operator==(const s_server& other) const
            {
                return bin == other.bin && port == other.port;
            }
        };

        struct s_loopback : module_description
        {
            std::string virtualSinkName = "VirtualSink";
            int framesPerBuffer = 1024;
            int sampleRate = 48000;
            int channels = 2;

            s_loopback()
            {
                name = "loopback";
            }

            s_loopback(const s_loopback& oth) = default;
            s_loopback& operator=(const s_loopback& oth) = default;

            bool operator==(const s_loopback& oth) const
            {
                return virtualSinkName == oth.virtualSinkName && name == oth.name &&
                    dependsOn == oth.dependsOn && framesPerBuffer == oth.framesPerBuffer && sampleRate == oth.sampleRate
                    && channels == oth.channels;
            }
        };

        struct s_router : module_description
        {
            struct s_source
            {
                std::string name;
                int framesPerBuffer = 1024;
                int sampleRate = 48000;
                int channels = 2;

                bool operator==(const s_source& oth) const
                {
                    return name == oth.name && framesPerBuffer == oth.framesPerBuffer && sampleRate == oth.sampleRate &&
                        channels == oth.channels;
                }
            };

            struct s_sink
            {
                std::string name = "/tmp/snapfifo";
                int framesPerBuffer = 1024;
                int sampleRate = 48000;
                int channels = 2;

                bool operator==(const s_sink& oth) const
                {
                    return name == oth.name && framesPerBuffer == oth.framesPerBuffer && sampleRate == oth.sampleRate &&
                        channels == oth.channels;
                }
            };

            s_router(): s_router({}, {})
            {
            }

            s_router(s_source source, s_sink sink): source(std::move(source)), sink(std::move(sink))
            {
                name = "router";
                dependsOn.assign({"loopback", "snapserver"});
            }

            s_router(const s_router& oth) = default;
            s_router& operator=(const s_router& oth) = default;

            s_source source{};
            s_sink sink{};

            bool operator==(const s_router& oth) const
            {
                return dependsOn == oth.dependsOn && source == oth.source && sink == oth.sink;
            }
        };

        struct s_snapserver : module_description
        {
            struct s_ports
            {
                int http = 1780;
                int stream = 1704;
                int control = 1705;

                bool operator==(const s_ports& oth) const
                {
                    return oth.http == http && oth.stream == stream && oth.control == control;
                }
            };

            std::string bin;
            std::string config;
            s_ports ports{};

            s_snapserver()
            {
                name = "snapserver";
            }

            s_snapserver(const s_snapserver& oth) = default;
            s_snapserver& operator=(const s_snapserver& oth) = default;

            bool operator==(const s_snapserver& oth) const
            {
                return dependsOn == oth.dependsOn && bin == oth.bin && config == oth.config &&
                    ports == oth.ports;
            }
        };

        struct s_snapclient : module_description
        {
            std::vector<std::string> dependsOn = {"snapserver"};
            std::string bin;
            int sinkIndex = -1;

            s_snapclient()
            {
                name = "snapclient";
                dependsOn.assign({"snapserver", "loopback"});
            }

            s_snapclient(const s_snapclient& oth) = default;
            s_snapclient& operator=(const s_snapclient& oth) = default;

            bool operator==(const s_snapclient& oth) const
            {
                return dependsOn == oth.dependsOn && name == oth.name &&
                    bin == oth.bin && sinkIndex == oth.sinkIndex;
            }
        };

        s_server server;
        s_router router;
        s_loopback loopback;
        s_snapserver snapserver;
        s_snapclient snapclient;
    };

    s_log log;
    s_module module;
    s_manager manager;
};

using ModuleParams = std::variant<
    settings::s_module::s_router,
    settings::s_module::s_loopback,
    settings::s_module::s_snapserver,
    settings::s_module::s_snapclient,
    settings::s_module::s_server
>;

inline module_description module_cast(const ModuleParams& params)
{
    return std::visit([](auto&& mod)
    {
        return static_cast<module_description>(mod);
    }, params);
}

#endif //SETTINGS_HPP
