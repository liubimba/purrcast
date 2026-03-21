import {useConfiguration} from "../../hooks/useConfiguration.ts";
import * as React from "react";

const ConnectionDisplay: React.FC<{ title: string, connected: boolean }> = ({title, connected}) => {
    return (
        <div>
            <label>{title}</label>
            <span>
                    {connected ? "Connected" : "Disconnected"}
            </span>
        </div>
    )
}

export const ServerBoard = () => {
    const {host, port, setHost, setPort, connected} = useConfiguration();

    return (
        <div>
            <div>
                <label>Host:</label>
                <input value={host} onChange={(e) => setHost(e.target.value)}/>
                <label>Port:</label>
                <input value={port} onChange={(e) => setPort(Number(e.target.value))}/>
            </div>
            <div>
                <ConnectionDisplay title={"Control"} connected={connected.control}/>
                <ConnectionDisplay title={"Snapcast"} connected={connected.snapcast}/>
            </div>
        </div>
    )
}