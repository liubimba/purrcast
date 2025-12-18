import {useDispatch, useSelector} from "react-redux";
import type {RootState} from "../store/store.ts";
import {useCallback} from "react";
import {userSlice} from "../store/userSlice.ts";

export const useClient = () => {
    const dispatch = useDispatch();
    const isLocal = useSelector((state: RootState) => state.user.isLocal);

    const setClientStarted = useCallback((started: boolean) => {
        dispatch({
            type: userSlice.actions.setStarted.type,
            payload: {
                started: started
            }
        })
    }, [dispatch]);
    return {
        isLocal: isLocal,
        setClientStarted: setClientStarted,
    }
}