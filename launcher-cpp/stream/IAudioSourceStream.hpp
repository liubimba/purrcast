//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOSOURCESTREAM_HPP
#define IAUDIOSOURCESTREAM_HPP
#include <mutex>
#include <unordered_set>

#include "IAudioSinkStream.hpp"

class IAudioSourceStream : public IAudioStream
{
public:
     explicit IAudioSourceStream(const Services* services, std::string id):
          IAudioStream(services, std::move(id))
     {
     }

     ~IAudioSourceStream() override = default;

     virtual void send(const AudioChunk& chunk) = 0;

     virtual void addSink(IAudioSinkStream* sink)
     {
          std::unique_lock lock(mutex_);
          sinks_.insert(sink);
     }

     virtual void removeSink(IAudioSinkStream* sink)
     {
          std::unique_lock lock(mutex_);
          if (sinks_.count(sink) > 0) sinks_.erase(sink);
     }

protected:
     std::mutex mutex_;
     std::shared_ptr<spdlog::logger> logger_;
     std::unordered_set<IAudioSinkStream*> sinks_;
};

#endif //IAUDIOSOURCESTREAM_HPP
