import {useState} from "react";
import {StartedContext} from "../context/StartedContext.tsx";

export const StartedProvider: React.FC<{ children: React.ReactNode }> = ({children}) => {
    const [started, setStarted] = useState<boolean>(false);

    const start = () => setStarted(true);

    return (
        <StartedContext.Provider value={{started, start}}>
            {children}
        </StartedContext.Provider>
    )
}
