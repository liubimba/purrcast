export const ConnectionStatus = {
    CONNECTING: "CONNECTING",
    CONNECTED: "CONNECTED",
    DISCONNECTED: "DISCONNECTED",
    FAILED: "FAILED"
} as const;


export type ConnectionStatus = typeof ConnectionStatus[keyof typeof ConnectionStatus];