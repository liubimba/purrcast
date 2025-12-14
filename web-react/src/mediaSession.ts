import snapcast from "./assets/snapcast-512.png";
import {SnapStream} from "./snapstream.ts";
import type {SnapControl} from "./snapcontrol.ts";
import silence from "./assets/10-seconds-of-silence.mp3";

export class MediaSession {
    private snapstream: SnapStream | null = null;
    private streamId: string | null = null;
    private snapcontrol: SnapControl;
    private audio: HTMLAudioElement;

    constructor(snapcontrol: SnapControl, streamId: string) {
        this.snapcontrol = snapcontrol;
        this.streamId = streamId;
        this.audio = new Audio();
    }

    setSnapstream(snapstream: SnapStream) {
        this.snapstream = snapstream;
    }

    setStreamId(streamId: string) {
        this.streamId = streamId;
    }

    connect(baseURL: string) {
        this.snapstream = new SnapStream(baseURL);
        this.snapstream.connect();
    }

    play() {
        if (this.snapstream) {
            console.log("AudioContext state: ", this.snapstream.ctx.state);
            this.snapstream.resume();
        }

        this.audio.src = silence;
        this.audio.loop = true;
        this.audio.play();
    }

    pause() {
        if (this.snapstream) {
            this.snapstream.pause();
        }
        this.audio.pause();
        this.audio.src = '';
    }

    stop() {
        this.pause();
        if (this.snapstream) {
            this.snapstream.stop();
        }
    }

    update() {
        if (!this.snapstream) {
            console.error("Undefined SnapStream");
            return;
        }

        if (!this.streamId) {
            console.error("Undefined StreamId");
            return;
        }

        try {
            console.debug("Updating media session...");

            const properties = this.snapcontrol.getStream(this.streamId).properties;
            const metadata = properties.metadata;
            const title: string = metadata?.title || "Unknown Title";
            const artist: string = metadata?.artist ? metadata.artist.join(", ") : "Unknown Artist";
            const album: string = metadata?.album || "";
            let artwork: Array<MediaImage> = [{src: snapcast, sizes: '512x512', type: 'image/png'}];
            if (metadata?.artUrl !== undefined) {
                artwork = [
                    {src: metadata.artUrl, sizes: '96x96', type: 'image/png'},
                    {src: metadata.artUrl, sizes: '128x128', type: 'image/png'},
                    {src: metadata.artUrl, sizes: '192x192', type: 'image/png'},
                    {src: metadata.artUrl, sizes: '256x256', type: 'image/png'},
                    {src: metadata.artUrl, sizes: '384x384', type: 'image/png'},
                    {src: metadata.artUrl, sizes: '512x512', type: 'image/png'},
                ]
            }

            navigator.mediaSession!.metadata = new MediaMetadata({
                title: title,
                artist: artist,
                album: album,
                artwork: artwork
            });

            const mediaSession = navigator.mediaSession!;
            let play_state: MediaSessionPlaybackState = "none";
            if (properties.playbackStatus !== undefined) {
                if (properties.playbackStatus === "playing") {
                    this.audio.play();
                    play_state = "playing";
                } else if (properties.playbackStatus === "paused") {
                    this.audio.pause();
                    play_state = "paused";
                } else if (properties.playbackStatus === "stopped") {
                    this.audio.pause();
                    play_state = "none";
                }
            }

            mediaSession.playbackState = play_state;
            mediaSession.setActionHandler('play', properties.canPlay ? () => {
                if (this.streamId) {
                    this.snapcontrol.control(this.streamId, 'play');
                }
            } : null);
            mediaSession.setActionHandler('pause', properties.canPause ? () => {
                if (this.streamId) {
                    this.snapcontrol.control(this.streamId, 'pause');
                }
            } : null);
            mediaSession.setActionHandler('previoustrack', properties.canGoPrevious ? () => {
                if (this.streamId) {
                    this.snapcontrol.control(this.streamId, 'previous');
                }
            } : null);
            mediaSession.setActionHandler('nexttrack', properties.canGoNext ? () => {
                if (this.streamId) {
                    this.snapcontrol.control(this.streamId, 'next');
                }
            } : null);
            mediaSession.setActionHandler('stop', properties.canControl ? () => {
                if (this.streamId) {
                    this.snapcontrol.control(this.streamId, 'stop');
                }
            } : null);
            const defaultSkipTime: number = 10; // Time to skip in seconds by default
            mediaSession.setActionHandler('seekbackward', properties.canSeek ?
                (event: MediaSessionActionDetails) => {
                    const offset: number = (event.seekOffset || defaultSkipTime) * -1;
                    if (properties.position !== undefined)
                        Math.max(properties.position! + offset, 0);
                    if (this.streamId) {
                        this.snapcontrol.control(this.streamId, 'seek', {'offset': offset})
                    }
                } : null);

            mediaSession.setActionHandler('seekforward', properties.canSeek ? (event: MediaSessionActionDetails) => {
                const offset: number = event.seekOffset || defaultSkipTime;
                if ((metadata?.duration !== undefined) && (properties.position !== undefined))
                    Math.min(properties.position! + offset, metadata.duration!);
                if (this.streamId) {
                    this.snapcontrol.control(this.streamId, 'seek', {'offset': offset})
                }
            } : null);

            mediaSession.setActionHandler('seekto', properties.canSeek ? (event: MediaSessionActionDetails) => {
                const position: number = event.seekTime || 0;
                if (metadata?.duration !== undefined)
                    Math.min(position, metadata.duration!);
                if (this.streamId) {
                    this.snapcontrol.control(this.streamId, 'setPosition', {'position': position})
                }
            } : null);

            if ((metadata?.duration !== undefined) && (properties.position !== undefined) && (properties.position! <= metadata.duration!)) {
                if ('setPositionState' in mediaSession) {
                    mediaSession.setPositionState!({
                        duration: metadata.duration,
                        playbackRate: 1.0,
                        position: properties.position!
                    });
                }
            } else {
                mediaSession.setPositionState!({
                    duration: 0,
                    playbackRate: 1.0,
                    position: 0
                });
            }

            console.debug("Successfully updated media session");
        } catch (e) {
            console.error("Failed to update media session. Error: ", e);
            return;
        }
    }
}