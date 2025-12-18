import type {RootState} from "../store.ts";
import {createSelector} from "@reduxjs/toolkit";

export const selectUser = (state: RootState) => state.user

export const selectUserStarted = () => createSelector(
    selectUser,
    (state) => state.started
)