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
import {MonitorService} from "../components/monitor/service/monitorService.ts";
import {ConnectionState, type ConnectionStatus, ConnectionStatusFactory} from "../shared/client/entity/status.ts";
import type {ModuleReport} from "../components/monitor/module/moduleReport.ts";

interface MonitorState {
    url: string;
    connectionStatus: ConnectionStatus;
    reports: ModuleReport[];
}

const name = "monitor";
const initialState: MonitorState = {
    url: "",
    connectionStatus: ConnectionStatusFactory.disconnected(),
    reports: [],
};

export const monitorSlice = createSlice({
    name,
    initialState,
    reducers: {
        connect: (state, action: PayloadAction<string>) => {
            state.url = action.payload;
        },
        disconnect: (state, _: PayloadAction<string>) => {
            state.url = initialState.url;
            state.reports = initialState.reports;
            state.connectionStatus = initialState.connectionStatus;
        },
        setConnectionStatus: (state, action: PayloadAction<ConnectionStatus>) => {
            const status: ConnectionStatus = action.payload as ConnectionStatus;
            if (state.connectionStatus.state === ConnectionState.FAILED && status.state == ConnectionState.DISCONNECTED) {
                return;
            }
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
        monitor.on("connectionStatus", (status: ConnectionStatus) => {
            store.dispatch({
                type: monitorSlice.actions.setConnectionStatus.type,
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
            } else if (configurationSlice.actions.connect.match(action)) {
                monitor.disconnect();
                store.dispatch({
                    type: monitorSlice.actions.disconnect.type
                })
            }
            return next(action);
        }
    }
}