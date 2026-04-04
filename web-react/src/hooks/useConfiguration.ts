import type {RootState} from "../store/store.ts";
import {useDispatch, useSelector} from "react-redux";
import {useCallback} from "react";
import {configurationSlice, type HostProps} from "../store/configurationSlice.ts";
import {type ConnectionStatus} from "../shared/client/entity/status.ts";

export const useConfiguration = () => {
    const dispatch = useDispatch();

    const host: HostProps = useSelector((state: RootState) => state.configuration.host);
    const controlServerConnection: ConnectionStatus = useSelector((state: RootState) => state.control.connectionStatus);
    const snapcastServerConnection: ConnectionStatus = useSelector((state: RootState) => state.snapcast.connectionStatus);
    const monitorServerConnection: ConnectionStatus = useSelector((state: RootState) => state.monitor.connectionStatus);
    const configurationServerConnection: ConnectionStatus = useSelector((state: RootState) => state.configuration.connectionStatus);

    const connect = useCallback((address: string, port: number) => {
        dispatch(configurationSlice.actions.connect({
            address: address,
            port: port
        }));
    }, [dispatch]);

    return {
        host, connect,
        connections: {
            controls: snapcastServerConnection,
            master: controlServerConnection,
            monitor: monitorServerConnection,
            configuration: configurationServerConnection,
        }
    }
}