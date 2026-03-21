import {useDispatch, useSelector} from "react-redux";
import type {RootState} from "../store/store.ts";
import {useCallback} from "react";
import {controlSlice} from "../store/controlSlice.ts";

export function useControl() {
    const dispatch = useDispatch();

    const host = useSelector((state: RootState) => state.control.url);
    const connected = useSelector((state: RootState) => state.control.connected);

    const setHost = useCallback((host: string) => {
        dispatch(controlSlice.actions.connect(host));
    }, [dispatch]);

    return {
        host,
        connected,
        setHost,
    }
}