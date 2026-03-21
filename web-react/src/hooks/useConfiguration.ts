import type {RootState} from "../store/store.ts";
import {useDispatch, useSelector} from "react-redux";
import {useCallback} from "react";
import {configurationSlice} from "../store/configurationSlice.ts";

export const useConfiguration = () => {
    const dispatch = useDispatch();

    const host = useSelector((state: RootState) => state.configuration.host);
    const port = useSelector((state: RootState) => state.configuration.port);
    const controlServerConnected = useSelector((state: RootState) => state.control.connected);
    const snapcastServerConnected = useSelector((state: RootState) => state.snapcast.connected);

    const setHost = useCallback((host: string) => {
        dispatch(configurationSlice.actions.setHost(host));
    }, [dispatch]);
    const setPort = useCallback((port: number) => {
        dispatch(configurationSlice.actions.setPort(port));
    }, [dispatch]);

    return {
        host, port,
        setHost, setPort,
        connected: {
            snapcast: snapcastServerConnected,
            control: controlServerConnected,
        }
    }
}