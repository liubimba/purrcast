import {configureStore} from "@reduxjs/toolkit";
import {controlSlice, createControlMiddleware} from "./controlSlice.ts";
import {masterPlayerSlice} from "./masterPlayerSlice.ts";
import {createSnapcastMiddleware, snapcastSlice} from "./snapcastSlice.ts";
import {configurationSlice, createConfigurationMiddleware} from "./configurationSlice.ts";

export const store = configureStore({
    reducer: {
        masterPlayer: masterPlayerSlice.reducer,
        snapcast: snapcastSlice.reducer,
        control: controlSlice.reducer,
        configuration: configurationSlice.reducer,
    },
    middleware: getDefaultMiddleware => getDefaultMiddleware()
        .concat(createConfigurationMiddleware())
        .concat(createControlMiddleware())
        .concat(createSnapcastMiddleware())
});

export type RootState = ReturnType<typeof store.getState>;
export type AppDispatch = typeof store.dispatch;