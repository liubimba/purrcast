import {
    createSlice,
    type Dispatch,
    type Middleware,
    type MiddlewareAPI,
    type PayloadAction,
    type UnknownAction
} from "@reduxjs/toolkit";
import {masterPlayerSlice} from "./masterPlayerSlice.ts";
import {ControlService} from "../services/ControlService.ts";
import {SnapStream} from "../shared/snapcast/snapstream.ts";
import AsyncLock from "async-lock"
import type {RootState} from "./store.ts";
import {userSlice} from "./userSlice.ts";
import {type ConnectionStatus, ConnectionStatusFactory} from "../shared/client/entity/status.ts";

interface ControlSliceState {
    url: string,
    connectionStatus: ConnectionStatus,
}

const name = "control"


const initialState: ControlSliceState = {
    url: "ws://127.0.0.1:8080" as string,
    connectionStatus: ConnectionStatusFactory.disconnected(),
}

export const controlSlice = createSlice({
    name: name,
    initialState: initialState,
    reducers: {
        connect: (state, action: PayloadAction<string>) => {
            state.url = action.payload;
        },
        setConnectionStatus: (state, action: PayloadAction<ConnectionStatus>) => {
            state.connectionStatus = action.payload;
        }
    }
})

export const createControlMiddleware = (): Middleware => {
    const service: ControlService = new ControlService(SnapStream.getClientId());
    const lock = new AsyncLock();

    return (store: MiddlewareAPI<Dispatch<UnknownAction>, RootState>) => {
        service.on("connectionStatus", (status: ConnectionStatus) => {
            store.dispatch(controlSlice.actions.setConnectionStatus(status));
        })
        service.on("masterPlayer", (volume: number, muted: boolean) => {
            store.dispatch({
                type: masterPlayerSlice.actions.setMasterState.type,
                payload: {
                    state: {
                        volume: volume,
                        muted: muted,
                    },
                    remote: true
                }
            })
        })
        service.on("userData", (isLocal: boolean) => {
            store.dispatch({
                type: userSlice.actions.setUserData.type,
                payload: {
                    isLocal: isLocal,
                }
            })
        })
        return (next) => (action) => {
            if (controlSlice.actions.connect.match(action)) {
                lock.acquire(controlSlice.actions.connect.type, async () => {
                    const url = action.payload;
                    if (!URL.canParse(url)) {
                        return;
                    }
                    if (!url.startsWith("ws") && !url.startsWith("wss")) {
                        return;
                    }
                    service.disconnect();
                    service.connect(url);
                });
            }

            if (masterPlayerSlice.actions.setMasterVolume.match(action) ||
                masterPlayerSlice.actions.setMasterMuted.match(action) ||
                masterPlayerSlice.actions.setMasterState.match(action)) {
                const result = next(action);
                if (service.connected() && !action.payload.remote) {
                    service.notifyMasterPlayerChanged(store.getState().masterPlayer.volume, store.getState().masterPlayer.muted)
                }
                return result;
            }
            if (userSlice.actions.setStarted.match(action)) {
                if (action.payload.started) {

                }
            }
            return next(action);
        }
    }
}