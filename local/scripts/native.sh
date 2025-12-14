#!/bin/bash

# Variables to store process PIDs
SNAPSERVER_PID=""
PAREC_PID=""
CLIENT_PID=""

function startup_snapserver() {
    snapserver_bin=snapserver
    config_filepath=../configuration/snapserver.conf

    echo "Startup snapserver: $snapserver_bin --config $config_filepath"
    snapserver --config ../configuration/snapserver.conf &
    SNAPSERVER_PID=$!
    echo "Snapserver started with PID: $SNAPSERVER_PID"
}

function cleanup_snapserver(){
    echo "Stopping snapserver..."
    if [ -n "$SNAPSERVER_PID" ]; then
        kill $SNAPSERVER_PID 2>/dev/null
        wait $SNAPSERVER_PID 2>/dev/null
        echo "Snapserver stopped"
    fi
}

function startup_client() {
    ../cmake-build-debug/bin/snapclient --soundcard 32 &
    CLIENT_PID=$!
    echo "Snapclient started with PID: $CLIENT_PID"
}

function cleanup_client(){
    echo "Stopping snapserver..."
    if [ -n "$CLIENT_PID" ]; then
        kill $CLIENT_PID 2>/dev/null
        wait $CLIENT_PID 2>/dev/null
        echo "Snapserver stopped"
    fi
}

function startup_loopback() {
    # Load PulseAudio null sink module
    pactl load-module module-null-sink
    # Set null sink as default audio output
    pactl set-default-sink null-sink
    echo "Loopback module loaded"
}

function cleanup_loopback() {
    echo "Stopping loopback..."
    # Unload PulseAudio null sink module
    pactl unload-module module-null-sink
}

function startup_router() {
    # Start parec to capture audio and pipe to snapfifo
    parec -d null-sink.monitor --format=s16le  --channels=2 --rate=48000 > /tmp/snapfifo &
    PAREC_PID=$!  # Fixed: use $! instead of $1 to get PID of last background process
    echo "parec started with PID: $PAREC_PID"
}

function cleanup_router() {
    echo "Stopping parec..."
    if [ -n "$PAREC_PID" ]; then
        kill $PAREC_PID 2>/dev/null
        wait $PAREC_PID 2>/dev/null
        echo "parec stopped"
    fi
}

function cleanup() {
    cleanup_router
    cleanup_loopback
    cleanup_snapserver
    cleanup_client
}

# Set trap to catch exit signals
trap cleanup EXIT INT TERM

# Start all services
startup_snapserver
startup_loopback
startup_router  # Add this call - it was missing!
startup_client

# Main script logic
echo "Script is running... Press Ctrl+C to exit"
sleep infinity