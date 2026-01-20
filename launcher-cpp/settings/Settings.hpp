//
// Created by bimba on 11/15/25.
//

#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <variant>
#include <boost/chrono/ceil.hpp>

#include "../libs/snapcast/common/json.hpp"
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
        struct s_monitor : module_description
        {
            std::string address;
            int interval;
            int port;

            s_monitor(): interval(0), port(0)
            {
                module_name = "monitor";
            }

            bool operator==(const s_monitor& other) const
            {
                return other.address == address && other.port == port && other.interval == interval;
            }

            std::string to_string() const override
            {
                nlohmann::json j = module_description::to_json();
                j["address"] = address;
                j["interval"] = interval;
                j["port"] = port;
                return j.dump();
            }
        };

        struct s_server : module_description
        {
            std::string bin;
            std::string static_dir;
            int port = -1;

            s_server()
            {
                module_name = "server";
            }

            s_server(const s_server& oth) = default;
            s_server& operator=(const s_server& oth) = default;

            bool operator==(const s_server& other) const
            {
                return bin == other.bin && port == other.port;
            }

            std::string to_string() const override
            {
                nlohmann::json j = module_description::to_json();
                j["bin"] = bin;
                j["port"] = port;
                j["static_dir"] = static_dir;
                return j.dump();
            }
        };

        struct s_loopback : module_description
        {
            std::string loopback_sink_name = "null";
            int frames_per_buffer = 512;
            int sample_rate = 48000;
            int channels = 2;
            bool set_as_default = true;
            bool resolve_origin_alsa_device = true;

            s_loopback()
            {
                module_name = "loopback";
            }

            s_loopback(const s_loopback& oth) = default;
            s_loopback& operator=(const s_loopback& oth) = default;

            bool operator==(const s_loopback& oth) const
            {
                return loopback_sink_name == oth.loopback_sink_name && module_name == oth.module_name &&
                    depends_on == oth.depends_on && frames_per_buffer == oth.frames_per_buffer && sample_rate == oth.
                    sample_rate
                    && channels == oth.channels;
            }

            std::string to_string() const override
            {
                nlohmann::json j = module_description::to_json();
                j["virtualSink"] = loopback_sink_name;
                j["name"] = module_name;
                j["dependsOn"] = depends_on;
                j["framesPerBuffer"] = frames_per_buffer;
                j["sampleRate"] = sample_rate;
                j["channels"] = channels;
                return j.dump();
            }
        };

        struct s_router : module_description
        {
            struct s_source
            {
                std::string name;
                int framesPerBuffer = 512;
                int sampleRate = 48000;
                int channels = 2;

                bool operator==(const s_source& oth) const
                {
                    return name == oth.name && framesPerBuffer == oth.framesPerBuffer && sampleRate == oth.sampleRate &&
                        channels == oth.channels;
                }

                nlohmann::json to_json() const
                {
                    nlohmann::json j;
                    j["name"] = name;
                    j["framesPerBuffer"] = framesPerBuffer;
                    j["sampleRate"] = sampleRate;
                    j["channels"] = channels;
                    return j;
                }
            };

            struct s_sink
            {
                std::string name = "/tmp/snapfifo";
                int framesPerBuffer = 512;
                int sampleRate = 48000;
                int channels = 2;

                bool operator==(const s_sink& oth) const
                {
                    return name == oth.name && framesPerBuffer == oth.framesPerBuffer && sampleRate == oth.sampleRate &&
                        channels == oth.channels;
                }

                nlohmann::json to_json() const
                {
                    nlohmann::json j;
                    j["name"] = name;
                    j["framesPerBuffer"] = framesPerBuffer;
                    j["sampleRate"] = sampleRate;
                    j["channels"] = channels;
                    return j;
                }
            };

            bool record_source = true;

            s_router(): s_router({}, {})
            {
            }

            s_router(s_source source, s_sink sink): source(std::move(source)), sink(std::move(sink))
            {
                module_name = "router";
                depends_on.assign({"loopback"});
            }

            s_router(const s_router& oth) = default;
            s_router& operator=(const s_router& oth) = default;

            s_source source{};
            s_sink sink{};

            bool operator==(const s_router& oth) const
            {
                return depends_on == oth.depends_on && source == oth.source && sink == oth.sink;
            }

            std::string to_string() const override
            {
                nlohmann::json j = module_description::to_json();
                j["source"] = source.to_json();
                j["sink"] = sink.to_json();
                return j.dump();
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

                nlohmann::json to_json() const
                {
                    nlohmann::json j;
                    j["http"] = http;
                    j["stream"] = stream;
                    j["control"] = control;
                    return j;
                }
            };

            std::string path_to_binary;
            std::string config;
            s_ports ports{};

            s_snapserver()
            {
                module_name = "snapserver";
                depends_on = {"router"};
            }

            s_snapserver(const s_snapserver& oth) = default;
            s_snapserver& operator=(const s_snapserver& oth) = default;

            bool operator==(const s_snapserver& oth) const
            {
                return depends_on == oth.depends_on && path_to_binary == oth.path_to_binary && config == oth.config &&
                    ports == oth.ports;
            }

            std::string to_string() const override
            {
                nlohmann::json j = module_description::to_json();
                j["bin"] = path_to_binary;
                j["config"] = config;
                j["ports"] = ports.to_json();
                return j.dump();
            }
        };

        struct s_snapclient : module_description
        {
            std::string path_to_binary;
            std::string soundcard;

            s_snapclient()
            {
                module_name = "snapclient";
                depends_on.assign({"snapserver", "loopback"});
            }

            s_snapclient(const s_snapclient& oth) = default;
            s_snapclient& operator=(const s_snapclient& oth) = default;

            bool operator==(const s_snapclient& oth) const
            {
                return depends_on == oth.depends_on && module_name == oth.module_name &&
                    path_to_binary == oth.path_to_binary && soundcard == oth.soundcard;
            }

            [[nodiscard]] std::string to_string() const override
            {
                nlohmann::json j = module_description::to_json();
                j["path_to_binary"] = path_to_binary;
                j["soundcard"] = soundcard;
                return j.dump();
            }
        };

        struct s_test_environment : module_description
        {
            s_loopback loopback;
            s_snapclient snapclient;
            std::string loopback_name;
            std::string loopback_monitor_description;

            s_test_environment()
            {
                module_name = "test_environment";
                depends_on = {"router"};
            }

            bool operator==(const s_test_environment& oth) const
            {
                return oth.loopback == loopback && oth.snapclient == snapclient;
            }

            [[nodiscard]] std::string to_string() const override
            {
                nlohmann::json j = to_json();
                j["name"] = module_name;
                return j.dump();
            }
        };


        s_server server;
        s_router router;
        s_loopback loopback;
        s_snapserver snapserver;
        s_snapclient snapclient;
        s_monitor monitor;
        s_test_environment test_environment;
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
    settings::s_module::s_server,
    settings::s_module::s_monitor,
    settings::s_module::s_test_environment
>;

inline const module_description& module_cast(const ModuleParams& params)
{
    return std::visit([](const auto& mod) -> const module_description&
    {
        return mod;
    }, params);
}

#endif //SETTINGS_HPP
