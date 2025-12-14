//
// Created by bimba on 11/16/25.
//

#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <functional>
#include <future>
#include <thread>
#include <queue>

#include "../logger/LoggerFactory.hpp"
#include "../services/Services.hpp"


struct ITask
{
    virtual ~ITask() = default;
    virtual void run() = 0;
};

template <typename T>
struct TaskImpl : ITask
{
    std::promise<T> promise;
    std::function<T(void)> func;
    std::shared_ptr<spdlog::logger> logger;

    TaskImpl(std::function<T(void)>&& func, const std::shared_ptr<spdlog::logger>& logger):
        func(std::move(func)), logger(logger)
    {
    }

    void run() override
    {
        try
        {
            logger->info("Run task");
            if constexpr (std::is_void_v<T>)
            {
                func();
                promise.set_value();
            }
            else
            {
                promise.set_value(func());
            }
            logger->info("Run task end");
        }
        catch (...)
        {
            logger->error("Failed to run task");
            promise.set_exception(std::current_exception());
        }
    }
};

class Executor
{
public:
    Executor(const Services* services, const std::string& id):
        services_(services)
    {
        logger_ = services->get<LoggerFactory>()->create("Executor-" + id);
        thread_ = std::thread(&Executor::run_, this);
    }

    ~Executor()
    {
        logger_->info("Destroying");
        {
            std::unique_lock<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();
        if (thread_.joinable())
            thread_.join();
        services_->get<LoggerFactory>()->drop(logger_);
    }


    bool joinable()
    {
        return thread_.joinable();
    }

    void join()
    {
        if (!joinable())
        {
            throw std::runtime_error("Executor already joined");
        }
        logger_->info("Join");
        {
            std::unique_lock<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();
        if (thread_.joinable())
        {
            logger_->info("Joining loop thread");
            thread_.join();
            logger_->info("Joined loop thread");
        }
        logger_->info("Joined");
    }

    template <typename T>
    std::future<T> post(std::function<T(void)> func)
    {
        if (!running_)
        {
            throw std::runtime_error("Executor already stopped");
        }
        logger_->info("Post new task");
        std::unique_lock<std::mutex> lock(mutex_);
        auto task = std::make_unique<TaskImpl<T>>(std::move(func), logger_);
        auto future = task->promise.get_future();
        tasks_.push(std::move(task));
        cv_.notify_one();
        return future;
    }

private:
    void run_()
    {
        logger_->info("[loop-thread]: consume tasks");
        while (running_)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]()
            {
                return !tasks_.empty() || !running_;
            });
            if (!running_)
            {
                break;
            }
            while (!tasks_.empty())
            {
                tasks_.front()->run();
                tasks_.pop();
            }
        }
        logger_->info("[loop-thread]: interrupted");
    }

    std::mutex mutex_;
    std::thread thread_;
    bool running_ = true;
    const Services* services_;
    std::condition_variable cv_;
    std::shared_ptr<spdlog::logger> logger_;
    std::queue<std::unique_ptr<ITask>> tasks_;
};


#endif //EXECUTOR_HPP
