import {
    createSlice,
    type Dispatch,
    type Middleware,
    type MiddlewareAPI,
    type PayloadAction,
    type UnknownAction
} from "@reduxjs/toolkit";
import type {RootState} from "./store.ts";
import {configurationSlice, type MonitorConfig} from "./configurationSlice.ts";
import {selectWebsocketUrl} from "./selectors/configurationSelector.ts";
import {MonitorService} from "../monitor/service/monitorService.ts";
import {ConnectionStatus} from "../shared/client/entity/status.ts";
import type {ModuleReport} from "../monitor/module/moduleReport.ts";

interface MonitorState {
    url: string;
    connectionStatus: ConnectionStatus;
    reports: ModuleReport[];
}

const name = "monitor";
const initialState: MonitorState = {
    url: "",
    connectionStatus: ConnectionStatus.DISCONNECTED,
    reports: [],
};

export const monitorSlice = createSlice({
    name,
    initialState,
    reducers: {
        connect: (state, action: PayloadAction<string>) => {
            state.url = action.payload;
        },
        connectionStatus: (state, action: PayloadAction<ConnectionStatus>) => {
            state.connectionStatus = action.payload;
        },
        setReports(state, action: PayloadAction<ModuleReport[]>) {
            state.reports = action.payload;
        }
    }
})

export const createMonitorMiddleware = (): Middleware => {
    return (store: MiddlewareAPI<Dispatch<UnknownAction>, RootState>) => {
        const monitor: MonitorService = new MonitorService();
        monitor.on("connectStatus", (status: ConnectionStatus) => {
            store.dispatch({
                type: monitorSlice.actions.connectionStatus.type,
                payload: status
            })
        })
        monitor.on("onReport", (reports: ModuleReport[]) => {
            store.dispatch({
                type: monitorSlice.actions.setReports.type,
                payload: reports
            })
        });
        return (next) => (action) => {
            if (monitorSlice.actions.connect.match(action)) {
                const url: string = action.payload;
                monitor.disconnect();
                monitor.connect(url);
            } else if (configurationSlice.actions.setMonitorConfiguration.match(action)) {
                const config: MonitorConfig = action.payload as MonitorConfig;
                const url = selectWebsocketUrl(store.getState(), config.port);
                if (url) {
                    monitor.connect(url);
                }
            }
            return next(action);
        }
    }
}