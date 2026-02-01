import {createSelector} from "@reduxjs/toolkit";
import type {RootState} from "../store.ts";

const selectHost = (state: RootState) => state.configuration.host;

export const selectWebsocketUrl = createSelector(
    [
        selectHost,
        (_: RootState, port: number) => port,
        (_: RootState, __: number, path?: string) => path || ""
    ],
    (host, port, path): string | null => {
        const urlString = `ws://${host}:${port}${path}`;
        return URL.canParse(urlString) ? urlString : null;
    }
);

