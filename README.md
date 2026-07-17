![purrcast](docs/preview.png)

![platform](https://img.shields.io/badge/platform-Linux-blue)
![license](https://img.shields.io/badge/license-GPL--3.0-green)
[![CI](https://github.com/liubimba/purrcast/actions/workflows/ci.yml/badge.svg)](https://github.com/liubimba/purrcast/actions/workflows/ci.yml)

**Play what your computer is playing on every speaker in the house — through the browser.**

Start purrcast on your Linux desktop, open `purrcast.local` on a phone, a tablet, a laptop, an old
handset you keep in a drawer. Each one becomes a speaker, in sync with the rest, and nobody has to
install anything: they open a link.

## Why not just use Snapcast

purrcast is built on [Snapcast][snapcast], and Snapcast is very good at the hard part — keeping
audio in step across a network. What it asks in return is that every listening device runs a
native `snapclient`. That is fine for a Raspberry Pi bolted to a shelf and useless for the phone
in your pocket.

purrcast serves the [Snapweb][snapweb] client from the machine already running the audio, adds an
mDNS name so nobody hunts for an IP, picks free ports so nothing collides, and starts the whole
stack from one desktop icon. The thing you tell a guest is "open purrcast.local".

## What you get

- **Any browser is a speaker.** FLAC, Opus and PCM are decoded in the page; output latency is
  measured and compensated so devices stay in step rather than merely close.
- **A volume knob per speaker**, from any device — the kitchen can be quieter than the study.
- **Controls on your phone's lock screen.** Track, artist and artwork land in the system media
  controls through the MediaSession API.
- **It finds itself.** The host announces `purrcast.local` over mDNS.
- **Nothing to configure.** Ports are taken from the 5000–9000 range at startup, whatever is free.
- **You can see what is wrong.** Every subsystem reports its own health to the UI, live.

## Install

Grab the `.deb` from [releases](https://github.com/liubimba/purrcast/releases) and open it. Then
launch purrcast; the control panel opens by itself.

Everyone else opens `http://purrcast.local:<port>` — the port is on the panel.

## Requirements

Linux with **PulseAudio or PipeWire**, which is where the audio is captured from. That is the one
real requirement, and it is on the host only: listening devices need a browser and nothing else.

macOS and Windows are not supported. Not "not yet" — the capture and the process handling are
written against Linux, and there is no port in progress.

## How it works

```
  your music player
         │  system audio
         ▼
  ┌─────────────┐   virtual sink     ┌──────────┐   /tmp/snapfifo   ┌────────────┐
  │  loopback   │ ─────────────────► │  router  │ ────────────────► │ snapserver │
  └─────────────┘   (PulseAudio)     └──────────┘                   └─────┬──────┘
                                                                          │
   browser ◄──────────── WebSocket: audio ────────────────────────────────┤
   browser ◄──────────── WebSocket: volume, metadata ─────────────────────┘
   browser ◄──────────── HTTP: the app, /api/config ──── backend-go
   browser ◄──────────── WebSocket: health ───────────── monitor
```

A C++ launcher owns the audio path and the processes: it creates a PulseAudio null-sink and makes
it the default output, so everything the system plays lands there; reads that sink's monitor
through PortAudio and writes it into snapserver's FIFO; then starts snapserver, a local snapclient
so the host plays too, the Go server, and a monitor. Each module declares what it depends on, is
health-checked, and is restarted when it stops answering.

The Go server is deliberately thin: it serves the built web app, answers `/api/config` with the
ports the browser needs, relays state between open tabs, and registers the mDNS name.

The browser does the interesting part. `snapstream.ts` speaks Snapcast's stream protocol, decodes
FLAC/Opus/PCM, and schedules playback against `AudioContext.outputLatency` so what you hear lines
up with the other speakers. A browser on the host itself only sends commands — the native
snapclient is already playing there.

| | |
|---|---|
| `launcher-cpp` | audio capture, process supervision, health checks (C++17) |
| `backend-go` | static files, `/api/config`, tab-to-tab relay, mDNS (Go) |
| `web-react` | the UI and the Snapcast client in the page (React 19, TypeScript) |
| `electron-app` | picks ports, spawns the launcher, opens the panel (Electron) |

### Checking the audio actually arrives

`--test.enabled true` brings up a second virtual sink and a second client, then fingerprints both
ends and compares them: RMS energy, zero-crossing rate and eight energy bands per chunk, scored
`0.6 × bands + 0.2 × zero-crossings + 0.2 × energy ratio`. Below 0.75 it reports a mismatch. It is
a heuristic, not an FFT — enough to catch silence, drift and mangled audio without a microphone.

## Building

```bash
sudo apt install libboost-all-dev libpulse-dev libasound2-dev pkg-config \
                 libavahi-client-dev libflac-dev libogg-dev libvorbis-dev libopus-dev

cmake -S launcher-cpp -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j$(nproc)
cd backend-go  && go build ./...
cd web-react   && npm ci && npm run build
```

Needs Node 20+ and Go 1.22+.

## Tests

```bash
cmake -S launcher-cpp -B build-test -DBUILD_PURRCAST_TESTS=ON -DBUILD_TESTING=OFF
cmake --build build-test -j$(nproc)
cd build-test && ctest
cd backend-go && go test ./...
```

Tests that need a machine rather than only code — a sound card, a PulseAudio daemon, a real
snapserver — skip themselves and say why. To run those too, point them at the binaries:

```bash
PURRCAST_SNAPSERVER_BINARY=build-test/bin/snapserver \
PURRCAST_SNAPSERVER_CONFIG=config/snapserver.conf \
PURRCAST_SNAPCLIENT_BINARY=build-test/bin/snapclient \
PURRCAST_SERVER_BINARY=backend-go/backend-go \
ctest
```

## Known limitations

- **Linux only**, and the host needs PulseAudio or PipeWire.
- **"Master volume" sets every speaker to the same level.** It is a broadcast, not a master gain,
  so it overwrites whatever each speaker was individually set to.
- **The web UI has no tests.** The C++ and Go sides do.
- **One stream.** Everything the machine plays goes to every speaker; there are no zones.

## License

GPL-3.0-or-later — see [LICENSE](LICENSE).

Built on [Snapcast][snapcast] and [Snapweb][snapweb] by Johannes Pohl, both GPL-3.0.
`snapstream.ts` and `snapcontrol.ts` are derived from Snapweb and carry its licence.

[snapcast]: https://github.com/badaix/snapcast
[snapweb]: https://github.com/badaix/snapweb
