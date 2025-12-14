import {useRef} from "react";
import type {Logger} from "../logger/Logger.ts";
import {LoggerFactory} from "../logger/LoggerFactory.ts";

export const useLogger = (name: string) => {
    const ref = useRef<Logger>(LoggerFactory.getLogger(name));
    return ref.current;
}