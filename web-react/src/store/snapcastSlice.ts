import {
    createSlice,
    type Dispatch,
    type Middleware,
    type MiddlewareAPI,
    type PayloadAction,
    type UnknownAction
} from "@reduxjs/toolkit";
import {type ISnapcastService, SnapcastServiceFactory} from "../services/SnapcastService.ts";
import {Snapcast} from "../shared/snapcast/snapcontrol.ts";
import {masterPlayerSlice} from "./masterPlayerSlice.ts";
import type {RootState} from "./store.ts";
import {userSlice} from "./userSlice.ts";
import {configurationSlice, type SnapserverConfig} from "./configurationSlice.ts";
import {selectWebsocketUrl} from "./selectors/configurationSelector.ts";
import {type ConnectionStatus, ConnectionStatusFactory} from "../shared/client/entity/status.ts";

const name = "snapcast";

interface SnapcastSliceState {
    url: string,
    snapserver: Snapcast.Server
    connectionStatus: ConnectionStatus,
}

const initialState: SnapcastSliceState = {
    url: "" as string,
    snapserver: Snapcast.getDefaultServer() as Snapcast.Server,
    connectionStatus: ConnectionStatusFactory.disconnected(),
}

export const snapcastSlice = createSlice({
    name: name,
    initialState: initialState,
    reducers: {
        connect: (state, action: PayloadAction<string>) => {
            state.url = action.payload as string;
        },
        setConnectionStatus: (state, action: PayloadAction<ConnectionStatus>) => {
            state.connectionStatus = action.payload;
        },
        setSnapserver: (state, action: PayloadAction<Snapcast.Server>) => {
            state.snapserver = action.payload;
        },
        setClientMuted: (_, __: PayloadAction<{ muted: boolean, clientId: string }>) => {
        },
        setClientVolume: (_, __: PayloadAction<{ volume: number, clientId: string }>) => {
        },
    }
})

export const createSnapcastMiddleware = (): Middleware => {
    return (store: MiddlewareAPI<Dispatch<UnknownAction>, RootState>) => {

        const createSnapcastService = (isClientLocal?: boolean): ISnapcastService => {
            const snapcast: ISnapcastService = isClientLocal != null ? SnapcastServiceFactory.create(isClientLocal.valueOf()) :
                SnapcastServiceFactory.stub();
            snapcast.on('connectionStatus', (status: ConnectionStatus) => {
                store.dispatch(snapcastSlice.actions.setConnectionStatus(status));
            })
            snapcast.on('snapserver', (snapserver: Snapcast.Server) => {
                store.dispatch(snapcastSlice.actions.setSnapserver(Snapcast.Mapper.toServer(snapserver)));
            });
            return snapcast;
        }

        let snapcast: ISnapcastService = createSnapcastService();

        const connect = (url: string): void => {
            snapcast.disconnect();
            if (!URL.canParse(url)) {
                return;
            }
            snapcast.connect(url);
        }


        return (next) => (action) => {
            if (snapcastSlice.actions.connect.match(action)) {
                connect(action.payload as string);
            }
            if (snapcastSlice.actions.setClientMuted.match(action)) {
                if (snapcast.connected) {
                    snapcast.setMutedClient(action.payload.clientId, action.payload.muted);
                }
            }

            if (snapcastSlice.actions.setClientVolume.match(action)) {
                if (snapcast.connected) {
                    snapcast.setVolumeClient(action.payload.clientId, action.payload.volume);
                }
            }
            if (masterPlayerSlice.actions.setMasterVolume.match(action)) {
                if (snapcast.connected) {
                    snapcast.setVolumeAllClients(action.payload.volume);
                }
            }
            if (masterPlayerSlice.actions.setMasterMuted.match(action)) {
                if (snapcast.connected) {
                    snapcast.setMutedAllClients(action.payload.muted);
                }
            }
            if (userSlice.actions.setUserData.match(action)) {
                if (snapcast.connected) {
                    snapcast.disconnect();
                }
                snapcast = createSnapcastService(action.payload.isLocal);
                snapcast.connect(store.getState().snapcast.url);
            }
            if (configurationSlice.actions.setSnapserverConfiguration.match(action)) {
                const config = action.payload as SnapserverConfig;
                const url = selectWebsocketUrl(store.getState(), config.ports.http);
                store.dispatch({
                    type: snapcastSlice.actions.connect.type,
                    payload: url
                });
            }
            return next(action);
        }
    }
}