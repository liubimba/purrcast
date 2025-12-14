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
#include "../path/Path.hpp"
#include "../services/Services.hpp"
#include "../logger/LoggerFactory.hpp"
#include "../util/LinuxProcess.hpp"
#include "../modules/snapserver/SnapserverModule.hpp"
#include "../modules/snapclient/SnapclientModule.hpp"
#include "../modules/loopback/AudioLoopbackModule.hpp"

#if defined(__linux__)
#include <netinet/in.h>
#include <sys/socket.h>
#include "../services/PulseMainloopService.hpp"
#endif
#include "../stream/PortAudioSourceStream.hpp"
#include "../stream/FIFOAudioSinkStream.hpp"
#include <random>
#include "../health_checker/InternalHealthChecker.hpp"
#include "../health_checker/TcpHealthChecker.hpp"
#include "../util/Port.hpp"
#include "../modules/manager/ModuleManager.hpp"
#include "../modules/server/ServerModule.hpp"

class TestData
{
    static TestData& instance()
    {
        static TestData* data = nullptr;
        if (data == nullptr)
        {
            data = new TestData();
            data->services_.add(std::make_shared<LoggerFactory>());
#if defined(__linux__)
            data->services_.add(std::make_shared<pulse::MainloopService>(&data->services_));
#endif
        }
        return *data;
    }

    Services services_;

public:
    static const Services* services()
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
};

#endif //TESTS_HPP
