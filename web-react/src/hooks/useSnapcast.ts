import {useDispatch, useSelector} from "react-redux";
import {type RootState} from "../store/store.ts";
import {useCallback} from "react";
import {snapcastSlice} from "../store/snapcastSlice.ts";

export const useSnapcast = () => {
    const dispatch = useDispatch();

    const connected = useSelector((state: RootState) => state.snapcast.connected);
    const snapserver = useSelector((state: RootState) => state.snapcast.snapserver);

    const setClientVolume = useCallback((clientId: string, volume: number) => {
        dispatch({
            type: snapcastSlice.actions.setClientVolume.type,
            payload: {
                clientId: clientId,
                volume: volume,
            }
        })
    }, [dispatch]);
    const setClientMuted = useCallback((clientId: string, muted: boolean) => {
        dispatch({
            type: snapcastSlice.actions.setClientMuted.type,
            payload: {
                clientId: clientId,
                muted: muted
            }
        })
    }, [dispatch]);

    return {
        setClientMuted: setClientMuted,
        setClientVolume: setClientVolume,
        snapserver: snapserver,
        connected: connected,
    };
}
