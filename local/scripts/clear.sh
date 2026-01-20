echo "Kill backend..."
ps aux | grep backend | awk '{print $2}' | xargs kill -9
echo "Kill snapserver..."
ps aux | grep snapserver | awk '{print $2}' | xargs kill -9
echo "Kill snapclient..."
ps aux | grep snapclient | awk '{print $2}' | xargs kill -9
echo "Unload module-null-sink..."
pactl unload-module module-null-sink
echo "Unload snd-aloop..."
#systemctl --user stop pipewire.socket pipewire-pulse.socket pipewire pipewire-pulse
#sudo modprobe -r snd-aloop
#systemctl --user start pipewire pipewire-pulse

