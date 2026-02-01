import {Logger} from "../shared/logger/logger.ts";
import {LoggerFactory} from "../shared/logger/loggerFactory.ts";
import {Client} from "../shared/client/service/client.ts";
import type {ClientEvents} from "../shared/client/events/clientEvents.ts";
import type {Message} from "../shared/client/message/message.ts";

interface ControlEvents extends ClientEvents {
    masterPlayer: (volume: number, muted: boolean) => void;
    userData: (isLocal: boolean) => void;
}


const ControlMessageType = {
    MASTER_PLAYER: 'MASTER_PLAYER',
    USER_DATA: 'USER_DATA'
};

interface MasterPlayerControlMessage extends Message {
    payload: { volume: number; muted: boolean };
}

interface UserDataControlMessage extends Message {
    payload: { is_local: boolean };
}

export class ControlService extends Client<ControlEvents> {
    _logger: Logger;
    private readonly _id: string;

    public constructor(id: string) {
        const logger = LoggerFactory.getLogger(`ControlService-${id}`);

        super(logger);

        this._id = id;
        this._logger = logger;

        this.on("onMessage", (message: Message) => {
            if (message.type === ControlMessageType.MASTER_PLAYER) {
                const masterPlayerMessage = message as MasterPlayerControlMessage;
                this.emit("masterPlayer", masterPlayerMessage.payload.volume, masterPlayerMessage.payload.muted);
            } else if (message.type === ControlMessageType.USER_DATA) {
                const userDataMessage = message as UserDataControlMessage;
                this.emit("userData", userDataMessage.payload.is_local);
            }
        })
    }

    public notifyMasterPlayerChanged(volume: number, muted: boolean) {
        if (this.connected()) {
            const message: MasterPlayerControlMessage = {
                type: ControlMessageType.MASTER_PLAYER,
                payload: {
                    volume: volume,
                    muted: muted
                }
            };
            this.send(JSON.stringify(message));
            this._logger.info("Notify master volume changed:", volume, ". Event: ", message);
            return;
        }
        throw new Error("ControlClient::send is unavailable, the channel is not configured")
    }

    public get id(): string {
        return this._id;
    }
}
