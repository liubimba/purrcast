import {createSlice, type PayloadAction} from "@reduxjs/toolkit";

const name = "user";
const initialState = {
    isLocal: false,
    started: false,
};

export const userSlice = createSlice({
    name: name,
    initialState: initialState,
    reducers: {
        setUserData: (state, action: PayloadAction<{ isLocal: boolean }>) => {
            state.isLocal = action.payload.isLocal;
        },
        setStarted: (state, action: PayloadAction<{ started: boolean }>) => {
            state.started = action.payload.started;
        }
    }
})