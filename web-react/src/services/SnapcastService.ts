import {Snapcast, SnapControl} from "../snapcontrol.ts";
import {SnapStream} from "../snapstream.ts";
import {Logger} from "../shared/logger/logger.ts";
import {LoggerFactory} from "../shared/logger/loggerFactory.ts";
import {EventEmitter} from "events";
import snapcast from "../assets/snapcast-512.png";
import silence from "../assets/10-seconds-of-silence.mp3";
import {ConnectionStatus} from "../shared/client/entity/status.ts";


interface ISnapcastServiceEvents {
    connectionStatus: (status: ConnectionStatus) => void;
    snapserver: (snapserver: Snapcast.Server) => void;
}

export interface ISnapcastService {
    on<U extends keyof ISnapcastServiceEvents>(
        event: U,
        listener: ISnapcastServiceEvents[U]
    ): this;

    emit<U extends keyof ISnapcastServiceEvents>(
        event: U,
        ...args: Parameters<ISnapcastServiceEvents[U]>
    ): boolean;

    connect(url: string): void;

    disconnect(): void;

    get connected(): boolean;

    setMutedClient(clientId: string, muted: boolean): void;

    setVolumeClient(clientId: string, volume: number): void;

    setVolumeAllClients(volume: number): void;

    setMutedAllClients(muted: boolean): void;
}

export class LocalSnapcastService extends EventEmitter implements ISnapcastService {
    protected _logger: Logger;
    protected _url: string | null = null;
    protected _connectionStatus: ConnectionStatus = ConnectionStatus.DISCONNECTED;
    protected _snapserver: Snapcast.Server = Snapcast.getDefaultServer();
    protected _snapcontrol: SnapControl = new SnapControl();

    public constructor() {
        super();
        this._onChange = this._onChange.bind(this);
        this._onConnectionChanged = this._onConnectionChanged.bind(this);

        this._logger = LoggerFactory.getLogger("LocalSnapcastService");
        this._snapcontrol.onChange = this._onChange;
        this._snapcontrol.onConnectionChanged = this._onConnectionChanged;
    }

    public connect(url: string) {
        this._url = url;
        this._connectionStatus = ConnectionStatus.CONNECTING;
        this._snapcontrol.connect(url);
    }

    public disconnect() {
        if (this.url !== null) {
            this._url = null;
            this._connectionStatus = ConnectionStatus.DISCONNECTED;
            this._snapcontrol.disconnect();
        }
    }

    public get connected(): boolean {
        return this._connectionStatus === ConnectionStatus.CONNECTED;
    }

    public get connectionStatus() {
        return this._connectionStatus;
    }

    public get url(): string | null {
        return this._url;
    }

    public get snapserver() {
        return this._snapserver;
    }

    public setMutedClient(clientId: string, muted: boolean): void {
        this._snapcontrol.setVolume(clientId, this._snapcontrol.getClient(clientId).config.volume.percent, muted);
    }

    public setVolumeClient(clientId: string, volume: number): void {
        console.log("client ", volume);
        this._snapcontrol.setVolume(clientId, volume, this._snapcontrol.getClient(clientId).config.volume.muted);
    }

    public setMutedAllClients(muted: boolean): void {
        this._snapserver.groups.forEach((group) => {
            group.clients.forEach((client) => {
                this.setMutedClient(client.id, muted);
            })
        })
    }

    public setVolumeAllClients(volume: number) {
        this._snapserver.groups.forEach((group) => {
            group.clients.forEach(client => {
                this.setVolumeClient(client.id, volume);
            })
        })
    }

    protected _onChange(_control: SnapControl, server: Snapcast.Server) {
        try {
            this.snapserver = Snapcast.Mapper.toServer(server);
        } catch (err) {
            this._logger.error("Failed to process snapserver changes. Error:", err);
        }
    };

    protected _onConnectionChanged(_control: SnapControl, connected: boolean, error: string | undefined): void {
        if (connected) {
            this._logger.info("Connected to:", this._url);
            this.connectionStatus = ConnectionStatus.CONNECTED;
        } else {
            this._logger.error("Failed to connect to:", this._url, ". Error: ", error ?? "undefined");
            this.snapserver = Snapcast.getDefaultServer();
            this.connectionStatus = ConnectionStatus.FAILED;
        }
    };

    private set connectionStatus(connectionStatus: ConnectionStatus) {
        this._connectionStatus = connectionStatus;
        this.emit('connectionStatus', connectionStatus);
    }

    private set snapserver(snapserver: Snapcast.Server) {
        this._snapserver = snapserver;
        this.emit('snapserver', snapserver);
    }
}

export class RemoteSnapcastService extends LocalSnapcastService {
    private _snapstream: SnapStream | null = null;
    protected _audio: HTMLAudioElement = new Audio();

    public constructor() {
        super();
        this._logger = LoggerFactory.getLogger("RemoteSnapcastService");
    }

    public connect(url: string) {
        super.connect(url);
        this._audio.src = silence;
        this._audio.loop = true;
        this._audio.play().then(() => {
            this._snapstream = new SnapStream(url);
        })
    }

    public disconnect() {
        super.disconnect();
        this._snapstream?.stop();
        this._audio.pause();
        this._audio.src = '';
    }

    protected override _onChange(_control: SnapControl, server: Snapcast.Server) {
        super._onChange(_control, server);
        try {
            this._update();
        } catch (err) {
            this._logger.error("Failed to process snapserver changes. Error:", err);
        }
    };

    protected override _onConnectionChanged(_control: SnapControl, connected: boolean, error: string | undefined): void {
        super._onConnectionChanged(_control, connected, error);
        if (!connected) {
            this._snapstream?.stop();
            this._snapstream = null;
        }
    };

    private _update() {
        if (!this._snapstream) {
            this._logger.error("Undefined SnapStream");
            return;
        }
        const streamId = this._snapcontrol.getStreamFromClient(SnapStream.getClientId()).id;
        if (!streamId) {
            this._logger.error("Undefined StreamId");
            return;
        }

        try {
            this._logger.debug("Updating media session...");

            const properties = this._snapcontrol.getStream(streamId).properties;
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
                    this._audio.play();
                    play_state = "playing";
                } else if (properties.playbackStatus === "paused") {
                    this._audio.pause();
                    play_state = "paused";
                } else if (properties.playbackStatus === "stopped") {
                    this._audio.pause();
                    play_state = "none";
                }
            }

            mediaSession.playbackState = play_state;
            mediaSession.setActionHandler('play', properties.canPlay ? () => {
                if (streamId) {
                    this._snapcontrol.control(streamId, 'play');
                }
            } : null);
            mediaSession.setActionHandler('pause', properties.canPause ? () => {
                if (streamId) {
                    this._snapcontrol.control(streamId, 'pause');
                }
            } : null);
            mediaSession.setActionHandler('previoustrack', properties.canGoPrevious ? () => {
                if (streamId) {
                    this._snapcontrol.control(streamId, 'previous');
                }
            } : null);
            mediaSession.setActionHandler('nexttrack', properties.canGoNext ? () => {
                if (streamId) {
                    this._snapcontrol.control(streamId, 'next');
                }
            } : null);
            mediaSession.setActionHandler('stop', properties.canControl ? () => {
                if (streamId) {
                    this._snapcontrol.control(streamId, 'stop');
                }
            } : null);
            const defaultSkipTime: number = 10; // Time to skip in seconds by default
            mediaSession.setActionHandler('seekbackward', properties.canSeek ?
                (event: MediaSessionActionDetails) => {
                    const offset: number = (event.seekOffset || defaultSkipTime) * -1;
                    if (properties.position !== undefined)
                        Math.max(properties.position! + offset, 0);
                    if (streamId) {
                        this._snapcontrol.control(streamId, 'seek', {'offset': offset})
                    }
                } : null);

            mediaSession.setActionHandler('seekforward', properties.canSeek ? (event: MediaSessionActionDetails) => {
                const offset: number = event.seekOffset || defaultSkipTime;
                if ((metadata?.duration !== undefined) && (properties.position !== undefined))
                    Math.min(properties.position! + offset, metadata.duration!);
                if (streamId) {
                    this._snapcontrol.control(streamId, 'seek', {'offset': offset})
                }
            } : null);

            mediaSession.setActionHandler('seekto', properties.canSeek ? (event: MediaSessionActionDetails) => {
                const position: number = event.seekTime || 0;
                if (metadata?.duration !== undefined)
                    Math.min(position, metadata.duration!);
                if (streamId) {
                    this._snapcontrol.control(streamId, 'setPosition', {'position': position})
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

            this._logger.debug("Successfully updated media session");
        } catch (e) {
            this._logger.error("Failed to update media session. Error: ", e);
            return;
        }
    }
}


export class StubSnapcastService implements ISnapcastService {
    get connected(): boolean {
        return false;
    }

    on<U extends keyof ISnapcastServiceEvents>(_: U, __: ISnapcastServiceEvents[U]): this {
        return this;
    }

    emit<U extends keyof ISnapcastServiceEvents>(_: U, ...__: Parameters<ISnapcastServiceEvents[U]>): boolean {
        return false;
    }

    connect(_: string): void {
    }

    disconnect(): void {
    }

    setMutedClient(_: string, __: boolean): void {
    }

    setVolumeClient(_: string, __: number): void {
    }

    setVolumeAllClients(_: number): void {
    }

    setMutedAllClients(_: boolean): void {
    }
}

export class SnapcastServiceFactory {
    public static stub(): ISnapcastService {
        return new StubSnapcastService();
    }

    public static create(isClientLocal: boolean): ISnapcastService {
        if (isClientLocal) {
            return new LocalSnapcastService();
        }
        return new RemoteSnapcastService();
    }
}
