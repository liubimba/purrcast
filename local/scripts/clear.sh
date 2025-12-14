ps aux | grep snapserver | awk '{print $2}' | xargs kill -9
ps aux | grep snapclient | awk '{print $2}' | xargs kill -9
pactl unload-module module-null-sink