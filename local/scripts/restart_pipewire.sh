systemctl --user stop pipewire.socket pipewire-pulse.socket pipewire pipewire-pulse
sudo modprobe -r snd-aloop
systemctl --user start pipewire pipewire-pulse
