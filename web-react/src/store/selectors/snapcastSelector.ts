import type {RootState} from "../store.ts";
import {createSelector} from "@reduxjs/toolkit";

export const selectSnapserver = (state: RootState) => state.snapcast.snapserver;

export const selectStreamById = (streamId: string) =>
    createSelector(
        selectSnapserver,
        (snapserver) => snapserver.streams.find(stream => stream.id === streamId) || undefined
    )

export const selectClientsByGroupId = (groupId: string, displayOffline: boolean = false) =>
    createSelector(
        selectSnapserver,
        (snapserver) => snapserver.groups.find(group => group.id === groupId)?.clients.filter(client => client.connected || displayOffline)
    )