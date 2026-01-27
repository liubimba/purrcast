//
// Created by bimba on 11/15/25.
//

#ifndef IPROCESS_HPP
#define IPROCESS_HPP
#include <string>

#include "../services/services.hpp"

enum class process_state
{
    NEW, EXECUTING, RUNNING, TERMINATING, TERMINATED, FAILED
};

typedef struct s_process_result
{
    bool is_ok;
    std::string message;

    static s_process_result ok()
    {
        return ok("");
    }

    static s_process_result ok(const std::string& message)
    {
        return s_process_result{.is_ok = true, .message = message};
    }

    static s_process_result failed()
    {
        return failed("");
    }

    static s_process_result failed(const std::string& message)
    {
        return s_process_result{.is_ok = false, .message = message};
    }
} process_result;

class i_process_listener;

class i_process
{
public:
    explicit i_process(const services* services, std::string id):
        services_(services), id_(std::move(id))
    {
    }

    virtual ~i_process() = default;
    virtual process_result terminate() = 0;
    virtual process_state state() = 0;
    virtual void add_listener(i_process_listener* listener) = 0;
    virtual process_result execute(const std::string& cmd, const std::string& args) = 0;

protected:
    std::string id_;
    const services* services_;
};


class i_process_listener
{
public:
    virtual ~i_process_listener() = default;
    virtual void onTerminate() = 0;
};

#endif //IPROCESS_HPP
