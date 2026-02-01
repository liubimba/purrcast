import type {Message} from "../message/message.ts";
import type {ConnectionStatus} from "../entity/status.ts";

export interface ClientEvents {
    connectionStatus: (status: ConnectionStatus) => void;
    onMessage: (event: Message) => void;
}
