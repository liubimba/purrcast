import {type Context, createContext} from "react";

export interface SnapserverPorts {
    http: number,
    control: number,
    stream: number
}

export interface SnapserverConfig {
    ports: SnapserverPorts;
}

export interface WebsocketConfig {
    path: string
    port: number
}

interface ConfigurationContextProps {
    host: string,
    port: number,
    websocket: WebsocketConfig,
    snapserver: SnapserverConfig,
    setHost: (url: string) => void,
    setPort: (port: number) => void,
    setWebsocket: (ports: WebsocketConfig) => void,
    setSnapserver: (snapserver: SnapserverConfig) => void
}

export const ConfigurationContext: Context<ConfigurationContextProps> = createContext<ConfigurationContextProps>({
    host: "0.0.0.0",
    port: 8080,
    websocket: {
        path: "/ws",
        port: 8080
    },
    snapserver: {
        ports: {
            http: 1780,
            stream: 1704,
            control: 1705
        }
    },
    setHost: () => null,
    setPort: () => null,
    setWebsocket: () => null,
    setSnapserver: () => null
})