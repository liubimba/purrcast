import {Snapcast, SnapControl} from "../snapcontrol.ts";
import {SnapStream} from "../snapstream.ts";
import {MediaSession} from "../mediaSession.ts";
import {Logger} from "../logger/Logger.ts";
import {LoggerFactory} from "../logger/LoggerFactory.ts";
import {Notifier} from "../notifier/notifier.ts";

export const ConnectionStatus = {
    CONNECTING: "CONNECTING",
    CONNECTED: "CONNECTED",
    DISCONNECTED: "DISCONNECTED",
    FAILED: "FAILED"
} as const;

export type ConnectionStatus = typeof ConnectionStatus[keyof typeof ConnectionStatus];

export const SnapcastNotificationType = {
    ON_CONNECTION_STATUS: "ON_CONNECTION_STATUS",
    ON_SNAPSERVER: "ON_SNAPSERVER",
} as const;

export type SnapcastNotificationType = typeof SnapcastNotificationType[keyof typeof SnapcastNotificationType];

export type ISnapcastNotificationPayload = Record<string, unknown>;

export type SnapcastConnectionStatusPayload = &ISnapcastNotificationPayload & {
    connectionStatus: ConnectionStatus;
}

export type SnapcastServerPayload = &ISnapcastNotificationPayload & {
    snapserver: Snapcast.Server
}

export class SnapcastService {
    private _logger: Logger;
    private _url: string | null = null;
    private _connectionStatus: ConnectionStatus = ConnectionStatus.DISCONNECTED;
    private _snapserver: Snapcast.Server = Snapcast.getDefaultServer();
    private _snapcontrol: SnapControl = new SnapControl();
    private _mediaSession: MediaSession;
    private _notifier: Notifier<SnapcastNotificationType, ISnapcastNotificationPayload>

    public constructor() {
        this._onChange = this._onChange.bind(this);
        this._onConnectionChanged = this._onConnectionChanged.bind(this);

        this._logger = LoggerFactory.getLogger("SnapcastService");
        this._notifier = new Notifier<SnapcastNotificationType, ISnapcastNotificationPayload>();
        this._mediaSession = new MediaSession(this._snapcontrol, SnapStream.getClientId());
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

    public on(type: SnapcastNotificationType, callback: (arg0: ISnapcastNotificationPayload) => void) {
        this._notifier.add(type, callback);
        return this;
    }

    public get connectionStatus() {
        return this._connectionStatus;
    }

    public get connected(): boolean {
        return this._connectionStatus === ConnectionStatus.CONNECTED;
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
        this._snapcontrol.setVolume(clientId, volume, this._snapcontrol.getClient(clientId).config.volume.muted);
    }

    private _onChange(_control: SnapControl, server: Snapcast.Server) {
        const streamId = this._snapcontrol.getStreamFromClient(SnapStream.getClientId()).id;
        this._mediaSession.setStreamId(streamId);
        this._mediaSession.update();
        this.snapserver = server;
    };

    private _onConnectionChanged(_control: SnapControl, connected: boolean, error: string | undefined): void {
        if (connected) {
            this._logger.info("Connected to:", this._url);
            this._mediaSession.connect(this._url!);
            this.connectionStatus = ConnectionStatus.CONNECTED;
        } else {
            this._logger.error("Failed to connect to:", this._url, ". Error: ", error ?? "undefined");
            this._mediaSession.pause();
            this.snapserver = Snapcast.getDefaultServer();
            this.connectionStatus = ConnectionStatus.FAILED;
        }
    };

    private set connectionStatus(connectionStatus: ConnectionStatus) {
        this._connectionStatus = connectionStatus;
        const statusNotification: SnapcastConnectionStatusPayload = {
            connectionStatus: this._connectionStatus,
        }
        this._notifier.notify(SnapcastNotificationType.ON_CONNECTION_STATUS, statusNotification);
    }

    private set snapserver(snapserver: Snapcast.Server) {
        this._snapserver = snapserver;
        const snapserverNotification: SnapcastServerPayload = {
            snapserver: snapserver,
        }
        this._notifier.notify(SnapcastNotificationType.ON_SNAPSERVER, snapserverNotification);
    }
}