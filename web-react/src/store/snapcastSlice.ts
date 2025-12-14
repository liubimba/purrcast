import {
    createSlice,
    type Dispatch,
    type Middleware,
    type MiddlewareAPI,
    type PayloadAction,
    type UnknownAction
} from "@reduxjs/toolkit";
import {
    ConnectionStatus,
    type ISnapcastNotificationPayload,
    type SnapcastConnectionStatusPayload,
    SnapcastNotificationType,
    type SnapcastServerPayload,
    SnapcastService
} from "../services/SnapcastService.ts";
import {Snapcast} from "../snapcontrol.ts";
import {masterPlayerSlice} from "./masterPlayerSlice.ts";
import type {RootState} from "./store.ts";

const name = "snapcast";

const initialState = {
    url: "" as string,
    connected: false as boolean,
    snapserver: Snapcast.getDefaultServer() as Snapcast.Server,
}

export const snapcastSlice = createSlice({
    name: name,
    initialState: initialState,
    reducers: {
        connect: (state, action: PayloadAction<string>) => {
            state.url = action.payload as string;
        },
        connected: (state) => {
            state.connected = true;
        },
        disconnected: (state) => {
            state.connected = false;
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
    const snapcast = new SnapcastService();
    const connect = (url: string): void => {
        snapcast.disconnect();
        if (!URL.canParse(url)) {
            return;
        }
        snapcast.connect(url);
    }

    return (store: MiddlewareAPI<Dispatch<UnknownAction>, RootState>) => {
        snapcast.on(SnapcastNotificationType.ON_CONNECTION_STATUS, (notification: ISnapcastNotificationPayload) => {
            const payload = notification as SnapcastConnectionStatusPayload;
            if (payload.connectionStatus === ConnectionStatus.CONNECTED) {
                store.dispatch(snapcastSlice.actions.connected());
            } else {
                store.dispatch(snapcastSlice.actions.disconnected());
            }
        })
        snapcast.on(SnapcastNotificationType.ON_SNAPSERVER, (notification: ISnapcastNotificationPayload) => {
            const payload = notification as SnapcastServerPayload;
            store.dispatch(snapcastSlice.actions.setSnapserver(Snapcast.Mapper.toServer(payload.snapserver)));
        })
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
                    snapcast.snapserver.groups.forEach(group => {
                        group.clients.forEach(client => {
                            snapcast.setVolumeClient(client.id, action.payload.volume);
                        })
                    })
                }
            }

            if (masterPlayerSlice.actions.setMasterMuted.match(action)) {
                if (snapcast.connected) {
                    snapcast.snapserver.groups.forEach(group => {
                        group.clients.forEach(client => {
                            snapcast.setMutedClient(client.id, action.payload.muted);
                        })
                    })
                }
            }
            return next(action);
        }
    }
}