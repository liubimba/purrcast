import {createSlice, type PayloadAction} from "@reduxjs/toolkit";

const name = "masterPlayer";

interface MasterPlayerState {
    volume: number;
    muted: boolean;
}

const initialState: MasterPlayerState = {
    volume: 0,
    muted: false
};

export const masterPlayerSlice = createSlice({
    name: name,
    initialState: initialState,
    reducers: {
        setMasterVolume(state, action: PayloadAction<{ volume: number, remote: boolean }>) {
            state.volume = action.payload.volume as number;
        },
        setMasterMuted(state, action: PayloadAction<{ muted: boolean, remote: boolean }>) {
            state.muted = action.payload.muted as boolean;
        },
        setMasterState(state, action: PayloadAction<{ state: MasterPlayerState, remote: boolean }>) {
            state.muted = action.payload.state.muted as boolean;
            state.volume = action.payload.state.volume as number;
        }
    }
});
