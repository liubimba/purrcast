import {useDispatch, useSelector} from "react-redux";
import type {RootState} from "../store/store.ts";
import {useCallback} from "react";
import {masterPlayerSlice} from "../store/masterPlayerSlice.ts";

export const useMasterPlayer = () => {
    const dispatch = useDispatch();
    const masterVolume = useSelector((state: RootState) => state.masterPlayer.volume);
    const masterMuted = useSelector((state: RootState) => state.masterPlayer.muted);

    const updateMasterVolume = useCallback((volume: number) => {
        dispatch(masterPlayerSlice.actions.setMasterVolume({
            volume: volume,
            remote: false,
        }));
    }, [dispatch]);

    const updateMasterMuted = useCallback((muted: boolean) => {
        dispatch(masterPlayerSlice.actions.setMasterMuted({
            muted: muted,
            remote: false
        }));
    }, [dispatch]);

    return {masterVolume, masterMuted, updateMasterVolume, updateMasterMuted}
}