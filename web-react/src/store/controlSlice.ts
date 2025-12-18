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
import {SnapStream} from "../snapstream.ts";
import {ConnectionStatus} from "../services/SnapcastService.ts";
import AsyncLock from "async-lock"
import type {RootState} from "./store.ts";
import {userSlice} from "./userSlice.ts";

const name = "control"

const initialState = {
    url: "ws://127.0.0.1:8080" as string,
    connected: false as boolean
}

export const controlSlice = createSlice({
    name: name,
    initialState: initialState,
    reducers: {
        connect: (state, action: PayloadAction<string>) => {
            state.url = action.payload;
        },
        connected: (state) => {
            state.connected = true;
        },
        disconnected: (state) => {
            state.connected = false;
        }
    }
})

export const createControlMiddleware = (): Middleware => {
    const service: ControlService = new ControlService(SnapStream.getClientId());
    const lock = new AsyncLock();

    return (store: MiddlewareAPI<Dispatch<UnknownAction>, RootState>) => {
        service.on("connectionStatus", (status: string) => {
            if (status === ConnectionStatus.CONNECTED) {
                store.dispatch(controlSlice.actions.connected());
            } else {
                store.dispatch(controlSlice.actions.disconnected());
            }
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
                    service.disconnectSafe();
                    const url = action.payload;
                    if (!URL.canParse(url)) {
                        return;
                    }
                    if (!url.startsWith("ws") && !url.startsWith("wss")) {
                        return;
                    }
                    if (service.connected || service.connecting) {
                        service.disconnectSafe();
                    }
                    service.connect(url);
                });
            }

            if (masterPlayerSlice.actions.setMasterVolume.match(action) ||
                masterPlayerSlice.actions.setMasterMuted.match(action) ||
                masterPlayerSlice.actions.setMasterState.match(action)) {
                const result = next(action);
                if (service.connected && !action.payload.remote) {
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