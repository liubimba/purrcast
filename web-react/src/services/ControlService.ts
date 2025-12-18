import {EventEmitter} from "events";
import {Logger} from "../logger/Logger.ts";
import {LoggerFactory} from "../logger/LoggerFactory.ts";
import {ConnectionStatus} from "./SnapcastService.ts";

type CloseEvent = WebSocketEventMap["close"];
type ErrorEvent = WebSocketEventMap["error"];

interface ControlServiceEvents {
    connectionStatus: (status: ConnectionStatus) => void;
    masterPlayer: (volume: number, muted: boolean) => void;
    userData: (isLocal: boolean) => void;
}

export interface IControlService {
    on<U extends keyof ControlServiceEvents>(
        event: U,
        listener: ControlServiceEvents[U]
    ): this;

    emit<U extends keyof ControlServiceEvents>(
        event: U,
        ...args: Parameters<ControlServiceEvents[U]>
    ): boolean;
}

export const IControlMessageType = {
    MASTER_PLAYER: 'MASTER_PLAYER',
    USER_DATA: 'USER_DATA'
} as const;


export type IControlMessageType =
    typeof IControlMessageType[keyof typeof IControlMessageType];

export type IControlMessage = {
    type: IControlMessageType;
    payload: object;
}

export interface MasterPlayerControlMessage extends IControlMessage {
    payload: { volume: number; muted: boolean };
}

export interface UserDataControlMessage extends IControlMessage {
    payload: { is_local: boolean };
}

export class ControlService extends EventEmitter implements IControlService {
    private _id: string
    private _logger: Logger;
    private _websocket: WebSocket | null = null;

    public constructor(id: string) {
        super();
        this._id = id;
        this._logger = LoggerFactory.getLogger("ControlService");
    }

    public connect(url: string | URL) {
        this._logger.info("Connecting to:", url);
        this._websocket = new WebSocket(url);
        this._websocket.onopen = (event: Event) => {
            this._logger.info("On open:", event);
            this.emit("connectionStatus", ConnectionStatus.CONNECTED);
        }
        this._websocket.onmessage = (messageEvent) => {
            this._logger.info("On message: ", messageEvent);
            const controlMessage = JSON.parse(messageEvent.data) as IControlMessage;
            if (controlMessage.type === IControlMessageType.MASTER_PLAYER) {
                const masterPlayerMessage = controlMessage as MasterPlayerControlMessage;
                this.emit("masterPlayer", masterPlayerMessage.payload.volume, masterPlayerMessage.payload.muted);
            } else if (controlMessage.type === IControlMessageType.USER_DATA) {
                const userDataMessage = controlMessage as UserDataControlMessage;
                this.emit("userData", userDataMessage.payload.is_local);
            }
        }
        this._websocket.onclose = (closeEvent: CloseEvent) => {
            this._logger.info("On close event: ", closeEvent);
            this.emit("connectionStatus", ConnectionStatus.DISCONNECTED);
        }
        this._websocket.onerror = (event: ErrorEvent) => {
            this._logger.error("On error:", event);
            this.emit("connectionStatus", ConnectionStatus.FAILED);
        }
        this.emit("connectionStatus", ConnectionStatus.CONNECTING);
    }

    public notifyMasterPlayerChanged(volume: number, muted: boolean) {
        if (this._websocket && this._websocket.readyState === WebSocket.OPEN) {
            const message: MasterPlayerControlMessage = {
                type: IControlMessageType.MASTER_PLAYER,
                payload: {
                    volume: volume,
                    muted: muted
                }
            };
            this._websocket.send(JSON.stringify(message));
            this._logger.info("Notify master volume changed:", volume, ". Event: ", message);
            return;
        }
        throw new Error("ControlClient::send is unavailable, the channel is not configured")
    }

    public get connecting(): boolean {
        return this._websocket != null && this._websocket.readyState === WebSocket.CONNECTING;
    }

    public get connected(): boolean {
        return this._websocket != null && this._websocket.readyState == WebSocket.OPEN;
    }

    public get url(): string | undefined {
        return this._websocket?.url;
    }

    public disconnect(): void {
        if (this._websocket && (this._websocket.readyState === WebSocket.OPEN || this._websocket.readyState === WebSocket.CONNECTING)) {
            this._logger.info("Disconnecting");
            this._websocket.close();
            this._websocket = null;
        }
        throw new ErrorEvent("ControlClient::disconnect is unavailable");
    }

    public disconnectSafe(): void {
        this._logger.info("Disconnecting");
        this._websocket?.close();
        this._websocket = null;
    }

    public closed(): boolean {
        return !this._websocket || this._websocket.readyState === WebSocket.CLOSED || this._websocket.readyState === WebSocket.CLOSING;
    }

    public getId(): string {
        return this._id;
    }
}
