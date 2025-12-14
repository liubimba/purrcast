//
// Created by bimba on 11/23/25.
//

#ifndef SETTINGSOBSERVABLE_HPP
#define SETTINGSOBSERVABLE_HPP
#include <functional>
#include <typeindex>
#include <unordered_set>
#include <boost/thread/thread.hpp>


class SettingsObservable
{
public:
    SettingsObservable();

    using Callback = std::function<void()>;

    template <typename T>
    void subscribe(const Callback& callback)
    {
        subscribers_.insert({typeid(T), callback});
    }

    void start();
    void stop();

private:
    void watch_();
    bool compare_();

    boost::mutex mutex_;
    std::atomic_bool running_;
    boost::thread watchThread_;
    boost::condition_variable condition_;
    boost::chrono::duration<double> interval_;
    std::unordered_map<std::type_index, Callback> subscribers_;
};


#endif //SETTINGSOBSERVABLE_HPP
