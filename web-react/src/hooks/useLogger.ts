import {useRef} from "react";
import type {Logger} from "../shared/logger/logger.ts";
import {LoggerFactory} from "../shared/logger/loggerFactory.ts";

export const useLogger = (name: string) => {
    const ref = useRef<Logger>(LoggerFactory.getLogger(name));
    return ref.current;
}