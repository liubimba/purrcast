type CloseEvent = WebSocketEventMap["close"];
type ErrorEvent = WebSocketEventMap["error"];

import {Logger} from "../logger/Logger.ts";
import {LoggerFactory} from "../logger/LoggerFactory.ts";
import {Notifier} from "../notifier/notifier.ts";
import {ConnectionStatus} from "./SnapcastService.ts";

export const IControlMessageType = {
    MASTER_PLAYER: 'MASTER_PLAYER',
    CONNECTION_STATUS: 'CONNECTION_STATUS',
    MESSAGE: 'MESSAGE',
} as const;

export type IControlMessageType =
    typeof IControlMessageType[keyof typeof IControlMessageType];


export type IControlMessageData = {
    type: IControlMessageType;
    payload: object;
}

export type IControlMessage = {
    user_id: string;
    message: IControlMessageData;
    internal_message_id: string;
}

export interface MasterPlayerControlMessage extends IControlMessage {
    message: {
        type: typeof IControlMessageType.MASTER_PLAYER;
        payload: { volume: number; muted: boolean };
    };
}

export interface ControlConnectionStatusMessage extends IControlMessage {
    message: {
        type: typeof IControlMessageType.CONNECTION_STATUS;
        payload: { status: ConnectionStatus };
    };
}

export type NotifyUnion = IControlMessage | MasterPlayerControlMessage;

export class ControlService {
    private _id: string
    private _logger: Logger;
    private _websocket: WebSocket | null = null;
    private _notifier: Notifier<IControlMessageType, NotifyUnion> = new Notifier<IControlMessageType, NotifyUnion>();

    public constructor(id: string) {
        this._id = id;
        this._logger = LoggerFactory.getLogger("ControlService");
    }

    public connect(url: string | URL) {
        this._logger.info("Connecting to:", url);
        this._websocket = new WebSocket(url);
        this._websocket.onopen = (event: Event) => {
            this._logger.info("On open:", event);
            this._notifier.notify(IControlMessageType.CONNECTION_STATUS, {
                user_id: this._id,
                internal_message_id: crypto.randomUUID(),
                message: {
                    type: IControlMessageType.CONNECTION_STATUS,
                    payload: {
                        status: ConnectionStatus.CONNECTED
                    }
                }
            })
        }
        this._websocket.onmessage = (messageEvent) => {
            this._logger.info("On message: ", messageEvent);
            const json = JSON.parse(messageEvent.data) as IControlMessage;
            this._notifier.notify(json.message.type, json);
        }
        this._websocket.onclose = (closeEvent: CloseEvent) => {
            this._logger.info("On close event: ", closeEvent);
            this._notifier.notify(IControlMessageType.CONNECTION_STATUS, {
                user_id: this._id,
                internal_message_id: crypto.randomUUID(),
                message: {
                    type: IControlMessageType.CONNECTION_STATUS,
                    payload: {
                        status: ConnectionStatus.DISCONNECTED
                    }
                }
            })
        }
        this._websocket.onerror = (event: ErrorEvent) => {
            this._logger.error("On error:", event);
            this._notifier.notify(IControlMessageType.CONNECTION_STATUS, {
                user_id: this._id,
                internal_message_id: crypto.randomUUID(),
                message: {
                    type: IControlMessageType.CONNECTION_STATUS,
                    payload: {
                        status: ConnectionStatus.FAILED
                    }
                }
            })
        }
        this._notifier.notify(IControlMessageType.CONNECTION_STATUS, {
            user_id: this._id,
            internal_message_id: crypto.randomUUID(),
            message: {
                type: IControlMessageType.CONNECTION_STATUS,
                payload: {
                    status: ConnectionStatus.CONNECTING
                }
            }
        })
    }

    public notifyMasterPlayerChanged(volume: number, muted: boolean) {
        if (this._websocket && this._websocket.readyState === WebSocket.OPEN) {
            const message: MasterPlayerControlMessage = {
                user_id: this._id,
                internal_message_id: crypto.randomUUID(),
                message: {
                    type: IControlMessageType.MASTER_PLAYER,
                    payload: {
                        volume: volume,
                        muted: muted
                    }
                }
            };
            this._websocket.send(JSON.stringify(message));
            this._logger.info("Notify master volume changed:", volume, ". Event: ", message);
            return;
        }
        throw new Error("ControlClient::send is unavailable, the channel is not configured")
    }

    public on(type: IControlMessageType, callback: (arg0: NotifyUnion) => void): ControlService {
        this._notifier.add(type, callback);
        return this;
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
