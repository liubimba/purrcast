import {createContext} from "react";

interface StartedContextType {
    started: boolean;
    start: () => void;
}

export const StartedContext = createContext<StartedContextType>({started: false, start: () => null});

