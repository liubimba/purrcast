import {
    createSlice,
    type Dispatch,
    type Middleware,
    type MiddlewareAPI,
    type PayloadAction,
    type UnknownAction
} from "@reduxjs/toolkit";
import {LoggerFactory} from "../logger/LoggerFactory.ts";
import type {Logger} from "../logger/Logger.ts";
import {snapcastSlice} from "./snapcastSlice.ts";
import {controlSlice} from "./controlSlice.ts";
import type {RootState} from "./store.ts";

interface ConfigResponse {
    snapserver: SnapserverConfig;
    websocket: WebsocketConfig;
}

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

export interface ConfigurationProps {
    host: string,
    port: number,
    websocket: WebsocketConfig,
    snapserver: SnapserverConfig,
}

const defaultSnapserverConfig = (): SnapserverConfig => {
    return {
        ports: {
            http: -1,
            control: -1,
            stream: -1
        }
    }
}

const defaultWebsocketConfig = (): WebsocketConfig => {
    return {
        path: "",
        port: -1
    }
}
const name: string = "configuration";
const initialState: ConfigurationProps = {
    host: window.location.hostname,
    port: Number(8080),
    websocket: defaultWebsocketConfig(),
    snapserver: defaultSnapserverConfig()
}

export const configurationSlice = createSlice({
    name: name,
    initialState: initialState,
    reducers: {
        setHost: (state, action: PayloadAction<string>) => {
            state.host = action.payload as string;
        },
        setPort: (state, action: PayloadAction<number>) => {
            state.port = action.payload as number;
        },
        setWebsocketConfiguration: (state, action: PayloadAction<WebsocketConfig>) => {
            state.websocket = action.payload as WebsocketConfig;
        },
        setSnapserverConfiguration: (state, action: PayloadAction<SnapserverConfig>) => {
            state.snapserver = action.payload as SnapserverConfig;
        },
        connect: () => {
        }
    }
})

export const createConfigurationMiddleware = (): Middleware => {
    const logger: Logger = LoggerFactory.getLogger("configuration-middleware");

    return (store: MiddlewareAPI<Dispatch<UnknownAction>, RootState>) => {
        const connect = (url: string) => {
            logger.info(`Fetching configuration from ${url}`);

            fetch(url)
                .then(res => res.json())
                .then((json: ConfigResponse) => {
                    logger.info("Received config:", json);

                    store.dispatch(
                        configurationSlice.actions.setSnapserverConfiguration(json.snapserver)
                    );

                    store.dispatch(
                        configurationSlice.actions.setWebsocketConfiguration(json.websocket)
                    );
                })
                .catch(err => {
                    logger.error("Failed to fetch config:", err);
                    store.dispatch(
                        configurationSlice.actions.setSnapserverConfiguration(defaultSnapserverConfig())
                    );
                    store.dispatch(
                        configurationSlice.actions.setWebsocketConfiguration(defaultWebsocketConfig())
                    );
                });
        };
        return (next) =>
            (action) => {


                if (configurationSlice.actions.setHost.match(action) ||
                    configurationSlice.actions.setPort.match(action)) {

                    const result = next(action);
                    const state = store.getState().configuration;

                    const api = `/api/config`;
                    const url = `http://${state.host}:${state.port}${api}`;

                    connect(url);
                    return result;
                }

                if (configurationSlice.actions.setSnapserverConfiguration.match(action)) {
                    const config = action.payload as SnapserverConfig;
                    const url = `ws://${store.getState().configuration.host}:${config.ports.http}`;

                    store.dispatch({
                        type: snapcastSlice.actions.connect.type,
                        payload: url
                    });
                }

                if (configurationSlice.actions.setWebsocketConfiguration.match(action)) {
                    const config = action.payload as WebsocketConfig;
                    const url = `ws://${store.getState().configuration.host}:${config.port}${config.path}`;

                    store.dispatch({
                        type: controlSlice.actions.connect.type,
                        payload: url
                    });
                }

                if (configurationSlice.actions.connect.match(action)) {
                    const state = store.getState().configuration;
                    const api = `/api/config`;
                    const url = `http://${state.host}:${state.port}${api}`;

                    connect(url);
                }

                return next(action);
            };
    };
};
