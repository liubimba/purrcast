import {EventEmitter} from "events";
import type {Logger} from "../../logger/logger.ts";
import type {ClientEvents} from "../events/clientEvents.ts";
import {ConnectionStatus} from "../entity/status.ts";
import type {Message} from "../message/message.ts";

type ValidEventMap<T> = {
    [K in keyof T]: (...args: any[]) => void;
};

export interface IClient<E extends ValidEventMap<E> & ClientEvents> {
    on<U extends Extract<keyof E, string>>(
        name: U,
        listener: E[U]
    ): this;

    emit<U extends Extract<keyof E, string>>(
        event: U,
        ...args: Parameters<E[U]>
    ): boolean;
}

type MessageData = string | ArrayBufferLike | Blob | ArrayBufferView;

export class Client<E extends ValidEventMap<E> & ClientEvents> extends EventEmitter implements IClient<E> {
    _logger: Logger;
    private _url: string | null = null;
    private _websocket: WebSocket | null = null;
    private _queue: MessageData[] = [];

    constructor(logger: Logger) {
        super();
        this._logger = logger;
    }

    public connect(url: string): void {
        this._logger.info("Connecting to:", url);
        this._websocket = new WebSocket(url);
        if (this._websocket == null) {
            throw Error(`Failed to connect: ${url}`);
        }
        this.emit("connectionStatus", ConnectionStatus.CONNECTING);
        this._websocket.onopen = (event: Event) => {
            this._logger.info("Connection opened:", event);
            this.emit("connectionStatus", ConnectionStatus.CONNECTED);
            this._queue.forEach((data: MessageData) => {
                this.send(data);
            })
        }
        this._websocket.onerror = (event: Event) => {
            this._logger.error("Connection error:", event);
            this.emit("connectionStatus", ConnectionStatus.FAILED);
        }
        this._websocket.onclose = (event: CloseEvent) => {
            this._logger.info("Connection closed:", event);
            this.emit("connectionStatus", ConnectionStatus.DISCONNECTED);
        }
        this._websocket.onmessage = (event: MessageEvent) => {
            this._logger.debug("On message:", event);
            this.emit("onMessage", JSON.parse(event.data) as Message);
        }
        this._url = url;
    }

    public disconnect(): void {
        if (this._websocket?.readyState == WebSocket.CONNECTING || this._websocket?.readyState == WebSocket.OPEN) {
            this._websocket.close(1000);
        }
        this._websocket = null;
    }

    public send(data: string | ArrayBufferLike | Blob | ArrayBufferView): void {
        if (this._websocket?.readyState == WebSocket.OPEN) {
            this._websocket.send(data);
        } else if (this._websocket?.readyState == WebSocket.CONNECTING) {
            this._queue.push(data);
        } else {
            throw Error("Client is connected for sending");
        }
    }

    public connected(): boolean {
        return this._websocket?.readyState === WebSocket.OPEN;
    }

    public get url(): string | null {
        return this._url;
    }
}