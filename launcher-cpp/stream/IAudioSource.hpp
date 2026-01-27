//
// Created by bimba on 11/20/25.
//

#ifndef IAUDIOSOURCE_HPP
#define IAUDIOSOURCE_HPP
#include <mutex>
#include <unordered_set>

#include "i_audio_sink.hpp"

class IAudioSource
{
public:
     virtual ~IAudioSource() = default;

     virtual void send(const audio_chunk& chunk) = 0;

     virtual void add_sink(i_audio_sink* sink)
     {
          std::unique_lock lock(mutex_);
          sinks_.insert(sink);
     }

     virtual void remove_sink(i_audio_sink* sink)
     {
          std::unique_lock lock(mutex_);
          if (sinks_.count(sink) > 0) sinks_.erase(sink);
     }

protected:
     std::mutex mutex_;
     std::unordered_set<i_audio_sink*> sinks_;
};

#endif //IAUDIOSOURCESTREAM_HPP
