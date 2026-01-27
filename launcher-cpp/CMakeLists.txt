cmake_minimum_required(VERSION 3.28)
project(multiroom VERSION 0.34.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_INSTALL_BINDIR ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_INSTALL_LIBDIR ${CMAKE_BINARY_DIR}/lib)

add_compile_definitions(VERSION="${PROJECT_VERSION}")

find_package(Boost 1.74 REQUIRED COMPONENTS thread chrono)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE REQUIRED libpulse)
pkg_check_modules(PULSEAUDIO REQUIRED libpulse)
if (PULSE_FOUND)
    add_definitions(-DHAS_PULSE)
    message(STATUS "PulseAudio found: ${PULSE_VERSION}")
else ()
    message(WARNING "PulseAudio not found")
endif ()

if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(yaml-cpp)


add_subdirectory(libs/json)
add_subdirectory(libs/spdlog)
add_subdirectory(libs/snapcast)
add_subdirectory(libs/portaudio)
add_subdirectory(libs/abseil-cpp)
add_subdirectory(libs/argparse)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_DEBUG_FLAGS "Enable debug flags and optimizations" OFF)
option(BUILD_TESTS "Build and run tests" OFF)

add_executable(multiroom
        launcher/launcher.cpp
        launcher/app.cpp

        logger/logger_factory.cpp
        modules/snapserver/snapserver_module.cpp
        modules/manager/module_manager.cpp
        util/os_port.cpp
        util/os_process.cpp
        util/linux_process.cpp
        util/task_executor.cpp
        modules/snapclient/snapclient_module.cpp
        util/process_info.cpp
        modules/loopback/audio_loopback_module.cpp
        modules/loopback/pulse_audio_loopback_module.cpp
        services/pulse_audio_mainloop_service.cpp
        stream/port_audio_source_stream.cpp
        stream/fifo_audio_sink_stream.cpp
        health_checker/internal_health_checker.cpp
        health_checker/tcp_health_checker.cpp
        modules/router/audio_router_module.cpp
        modules/server/server_module.cpp
        modules/monitor/monitor_module.cpp
        modules/monitor/monitor_session.cpp
        modules/test_environment/test_environment.cpp
        stream/file_audio_sink_stream.cpp
        modules/test_environment/audio_inspector/fingerprint_audio_inspector.cpp
        modules/test_environment/listener/proxy_audio_sink_listener.cpp
        stream/audio_stream_result.hpp
)

set_target_properties(snapserver PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
set_target_properties(snapclient PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

set_target_properties(common PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

add_dependencies(multiroom snapserver snapclient)

target_include_directories(common PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/snapcast)
target_include_directories(snapserver PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/snapcast/server/
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/snapcast)

target_include_directories(multiroom PUBLIC
        ${PULSEAUDIO_INCLUDE_DIRS}
)

target_link_libraries(multiroom PUBLIC
        ${PULSEAUDIO_LIBRARIES}
        nlohmann_json::nlohmann_json
        yaml-cpp::yaml-cpp
        portaudio
        absl::strings
        absl::str_format
        absl::time
        spdlog::spdlog
        asound
        Boost::thread
        Boost::chrono
        argparse::argparse
)

if (ENABLE_DEBUG_FLAGS)
    target_compile_options(multiroom PRIVATE
            -g -O0 -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor
            -Wcast-align -Wunused -Woverloaded-virtual -Wconversion
            -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2
    )
endif ()

if (ENABLE_ASAN)
    target_compile_options(multiroom PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_libraries(multiroom PRIVATE -fsanitize=address asan)
endif ()

if (ENABLE_UBSAN)
    target_compile_options(multiroom PRIVATE -fsanitize=undefined)
    target_link_libraries(multiroom PRIVATE -fsanitize=undefined)
endif ()

if (ENABLE_TSAN AND NOT ENABLE_ASAN)
    target_compile_options(multiroom PRIVATE -fsanitize=thread)
    target_link_libraries(multiroom PRIVATE -fsanitize=thread)
endif ()

if (RUN_TESTS)
    include(FetchContent)
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(multiroom_tests
            tests/ServicesTests.cpp
            tests/SettingsTests.cpp
            tests/LoggerFactoryTests.cpp
            tests/PortTests.cpp
            tests/LinuxProcessTests.cpp
            tests/ExecutorTests.cpp
            tests/SnapserverModuleTests.cpp
            tests/SnapclientModuleTests.cpp
            tests/AudioLoopbackModuleTests.cpp
            tests/PortAudioSourceStreamTests.cpp
            tests/FIFOAudioSinkStreamTests.cpp
            tests/InternalHealthCheckerTests.cpp
            tests/TCPHealthCheckerTests.cpp
            tests/ModuleManagerTests.cpp
            tests/AudioRouterModuleTests.cpp
            tests/PulseMainloopServiceTests.cpp
            tests/ServerModuleTests.cpp

            util/os_port.cpp
            util/linux_process.cpp
            util/os_process.cpp
            util/process_info.cpp
            util/task_executor.cpp
            launcher/launcher.cpp
            logger/logger_factory.cpp
            modules/manager/module_manager.cpp
            modules/snapclient/snapclient_module.cpp
            modules/snapserver/snapserver_module.cpp
            services/pulse_audio_mainloop_service.cpp
            modules/loopback/audio_loopback_module.cpp
            modules/router/audio_router_module.cpp
            modules/loopback/pulse_audio_loopback_module.cpp
            modules/server/server_module.cpp
            stream/port_audio_source_stream.cpp
            stream/fifo_audio_sink_stream.cpp
            health_checker/tcp_health_checker.cpp
            health_checker/internal_health_checker.cpp
    )

    target_link_libraries(multiroom_tests PUBLIC
            nlohmann_json::nlohmann_json
            yaml-cpp::yaml-cpp
            GTest::gtest_main
            GTest::gmock
            GTest::gmock_main
            absl::strings
            absl::str_format
            absl::time
            ${PULSEAUDIO_LIBRARIES}
            portaudio
            asound
            spdlog::spdlog
            Boost::chrono
            Boost::thread
            argparse::argparse
    )

    target_include_directories(multiroom_tests PUBLIC
            ${PULSEAUDIO_INCLUDE_DIRS}
            portaudio/include
            snapcast/
            json/include
            spdlog/include
            yaml-cpp/include

            tests/
            settings/
    )

    add_dependencies(multiroom_tests multiroom)

    include(GoogleTest)
    gtest_discover_tests(multiroom_tests)

    add_custom_target(run_tests
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            DEPENDS multiroom_tests
    )
else ()
    message(STATUS "Build tests disabled")
endif ()
