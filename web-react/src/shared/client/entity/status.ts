export const ConnectionState = {
    CONNECTING: "CONNECTING",
    FAILED: "FAILED",
    CONNECTED: "CONNECTED",
    DISCONNECTED: "DISCONNECTED",
} as const;

export type ConnectionState =
    (typeof ConnectionState)[keyof typeof ConnectionState];

export interface ConnectionStatus {
    state: ConnectionState;
    message?: string;
}

export class ConnectionStatusFactory {
    static connected(message?: string): ConnectionStatus {
        return {state: ConnectionState.CONNECTED, message: message};
    }

    static disconnected(message?: string): ConnectionStatus {
        return {state: ConnectionState.DISCONNECTED, message: message};
    }

    static connecting(message?: string): ConnectionStatus {
        return {state: ConnectionState.CONNECTING, message: message};
    }

    static failed(message?: string): ConnectionStatus {
        return {state: ConnectionState.FAILED, message: message};
    }
}
