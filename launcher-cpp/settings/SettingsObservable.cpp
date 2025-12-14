//
// Created by bimba on 11/23/25.
//

#include "SettingsObservable.hpp"

SettingsObservable::SettingsObservable():
    running_(false),
    interval_(boost::chrono::milliseconds(500)),
    subscribers_({})
{
}

void SettingsObservable::start()
{
    running_.store(true);
    watchThread_ = boost::thread(&SettingsObservable::watch_, this);
}

void SettingsObservable::stop()
{
    running_.store(false);
    condition_.notify_all();
    if (watchThread_.joinable()) watchThread_.join();
}

void SettingsObservable::watch_()
{
    while (running_.load())
    {
        boost::unique_lock lock(mutex_);
        condition_.wait_for(lock, interval_, [=]
        {
            return !running_.load();
        });
        if (!running_.load()) break;
        compare_();
    }
}

bool SettingsObservable::compare_()
{
}
