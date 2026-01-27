//
// Created by bimba on 12/23/25.
//

#ifndef MODULESTATUS_HPP
#define MODULESTATUS_HPP
#include <cstdint>
#include <string>
#include <utility>

#include "module_description.hpp"
#include "../settings/settings.hpp"

enum class Severity
{
    MINOR, NORMAL, IMPORTANT, CRITICAL, BLOCK
};

enum StatusCode
{
    OK = 0
};

typedef struct s_Status
{
    StatusCode code;
    Severity severity;
    std::string advice;
    std::string message;
    uint64_t timestampNs;
    module_params params;
} ModuleStatus;

class module_status
{
    std::string state_;
    std::string message_;
    uint64_t timestamp_;

    static uint64_t current_time_()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

public:
    explicit module_status(std::string state, std::string message): module_status(std::move(state), std::move(message), current_time_())
    {
    }

    explicit module_status(std::string state, std::string message, uint64_t timestamp_ns):
        state_(std::move(state)), message_(std::move(message)), timestamp_(timestamp_ns)
    {
    }

    virtual ~module_status() = default;

    [[nodiscard]] std::string get_state() const
    {
        return state_;
    }

    [[nodiscard]] std::string get_message() const
    {
        return message_;
    }

    [[nodiscard]] uint64_t get_timestamp() const
    {
        return timestamp_;
    }

    [[nodiscard]] nlohmann::json to_json() const
    {
        nlohmann::json json;
        json["state"] = state_;
        json["message"] = message_;
        json["timestamp_"] = timestamp_;
        return json;
    }

    [[nodiscard]] std::string to_string() const
    {
        return absl::StrFormat("[%s] %s", state_, message_);
    }

    [[nodiscard]] std::string stringify() const
    {
        return to_json().dump();
    }

    bool operator==(const module_status& oth) const
    {
        return state_ == oth.state_ && message_ == oth.message_ && timestamp_ == oth.timestamp_;
    }

    static module_status pending()
    {
        return module_status{"pending", ""};
    }

    static module_status established()
    {
        return module_status{"established", ""};
    }

    static module_status loaded(const std::string& message)
    {
        return module_status{"running", message};
    }

    static module_status failed(const std::string& message)
    {
        return module_status{"failed", message};
    }

    static module_status loading()
    {
        return loading("initializing");
    }

    static module_status loading(const std::string& message)
    {
        return module_status{"initializing", message};
    }

    static module_status unloading()
    {
        return unloading("");
    }

    static module_status unloading(const std::string& message)
    {
        return module_status{"unloading", message};
    }

    static module_status unloaded()
    {
        return unloaded("");
    }

    static module_status unloaded(const std::string& message)
    {
        return module_status{"unloaded", message};
    }
};


#endif //MODULESTATUS_HPP
