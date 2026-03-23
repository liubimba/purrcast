import type {RootState} from "../store/store.ts";
import {useDispatch, useSelector} from "react-redux";
import {useCallback} from "react";
import {configurationSlice} from "../store/configurationSlice.ts";
import {type ConnectionStatus} from "../shared/client/entity/status.ts";

export const useConfiguration = () => {
    const dispatch = useDispatch();

    const host: string = useSelector((state: RootState) => state.configuration.host);
    const port: number = useSelector((state: RootState) => state.configuration.port);
    const controlServerConnection: ConnectionStatus = useSelector((state: RootState) => state.control.connectionStatus);
    const snapcastServerConnection: ConnectionStatus = useSelector((state: RootState) => state.snapcast.connectionStatus);
    const monitorServerConnection: ConnectionStatus = useSelector((state: RootState) => state.monitor.connectionStatus);
    const configurationServerConnection: ConnectionStatus = useSelector((state: RootState) => state.configuration.connectionStatus);

    const setHost = useCallback((host: string) => {
        dispatch(configurationSlice.actions.setHost(host));
    }, [dispatch]);
    const setPort = useCallback((port: number) => {
        dispatch(configurationSlice.actions.setPort(port));
    }, [dispatch]);

    return {
        host, port,
        setHost, setPort,
        connections: {
            controls: snapcastServerConnection,
            master: controlServerConnection,
            monitor: monitorServerConnection,
            configuration: configurationServerConnection,
        }
    }
}