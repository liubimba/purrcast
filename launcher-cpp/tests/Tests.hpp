//
// Created by bimba on 11/15/25.
//

#ifndef TESTS_HPP
#define TESTS_HPP

#include "spdlog/sinks/base_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "absl/strings/match.h"
#include "../services/services.hpp"
#include "../logger/logger_factory.hpp"
#include "../util/linux_process.hpp"
#include "../modules/snapserver/snapserver_module.hpp"
#include "../modules/snapclient/snapclient_module.hpp"
#include "../modules/loopback/audio_loopback_module.hpp"

#if defined(__linux__)
#include <netinet/in.h>
#include <sys/socket.h>
#include "../services/pulse_audio_mainloop_service.hpp"
#endif
#include "../stream/port_audio_source_stream.hpp"
#include "../stream/fifo_audio_sink_stream.hpp"
#include <random>
#include "../health_checker/internal_health_checker.hpp"
#include "../health_checker/tcp_health_checker.hpp"
#include "../util/os_port.hpp"
#include "../modules/manager/module_manager.hpp"
#include "../modules/server/server_module.hpp"

class TestData
{
    static TestData& instance()
    {
        static TestData* data = nullptr;
        if (data == nullptr)
        {
            data = new TestData();
            data->services_.add(std::make_shared<logger_factory>());
#if defined(__linux__)
            data->services_.add(std::make_shared<pulse::mainloop_service>(&data->services_));
#endif
        }
        return *data;
    }

    services services_;

public:
    static const services* get_services()
    {
        return &instance().services_;
    };

    static std::string uuid(size_t length = 4)
    {
        const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()";
        std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);
        std::random_device rd;
        std::mt19937 generator(rd());
        std::string randomString(length, ' '); // Initialize with placeholder characters
        std::generate_n(randomString.begin(), length, [&]()
        {
            return CHARACTERS[distribution(generator)];
        });
        return randomString;;
    }

    static std::chrono::duration<double> timeout()
    {
        return std::chrono::seconds(1);
    }

    static std::string from_environment(const char* variable)
    {
        const char* value = std::getenv(variable);
        return value == nullptr ? std::string{} : std::string{value};
    }
};

class needs_audio_input_device : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Pa_Initialize();
        const PaDeviceIndex device = Pa_GetDefaultInputDevice();
        if (device == paNoDevice)
            GTEST_SKIP() << "no audio input device on this machine";
        const PaDeviceInfo* info = Pa_GetDeviceInfo(device);
        if (info == nullptr)
            GTEST_SKIP() << "no audio input device on this machine";
        device_name_ = info->name;
    }

    std::string device_name_;
};

class needs_pulse_audio : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!pulse::mainloop_service{TestData::get_services()}.available())
            GTEST_SKIP() << "no PulseAudio daemon on this machine";
    }
};

class needs_external_binary : public ::testing::Test
{
protected:
    explicit needs_external_binary(const char* variable): variable_(variable)
    {
    }

    void SetUp() override
    {
        binary_ = TestData::from_environment(variable_);
        if (binary_.empty())
            GTEST_SKIP() << "spawns a real process; set " << variable_ << " to its executable to run this";
    }

    std::string binary_;

private:
    const char* variable_;
};

#endif //TESTS_HPP
