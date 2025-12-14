import * as React from "react";
import {Logger} from "../logger/Logger.ts";
import {useLogger} from "../hooks/useLogger.ts";
import {useDispatch, useSelector} from "react-redux";
import type {AppDispatch, RootState} from "../store/store.ts";
import {configurationSlice} from "../store/configurationSlice.ts";

interface SidebarProps {
    onNavigate: (page: string) => void;
    page: string;
}

const Connected: React.FC<{}> = ({}) => {
    return (
        <div className="text-center">
            Connected
        </div>
    )
}

const Disconnected: React.FC<{}> = ({}) => {
    return (
        <div className="text-center">
            Disconnected
        </div>
    )
}

export const Sidebar: React.FC<SidebarProps> = ({onNavigate, page}) => {
    const dispatch: AppDispatch = useDispatch();
    const logger: Logger = useLogger("Sidebar");
    const host = useSelector((state: RootState) => state.configuration.host);
    const port = useSelector((state: RootState) => state.configuration.port);
    const snapcastConnected = useSelector((state: RootState) => state.snapcast.connected);
    const websocketConnect = useSelector((state: RootState) => state.control.connected);

    const navigationItems = [
        {name: "Main", icon: undefined, page: "main"},
        {name: "Settings", icon: undefined, page: "settings"}
    ];
    const onChangeHost = (event: React.ChangeEvent<HTMLInputElement>) => {
        logger.info("Typed server url: ", event.target.value);
        dispatch(configurationSlice.actions.setHost(event.target.value));
    };
    const onChangePort = (event: React.ChangeEvent<HTMLInputElement>) => {
        logger.info("Changed port: ", event.target.value);
        dispatch(configurationSlice.actions.setPort(Number(event.target.value)));
    };

    const active = {
        backgroundColor: 'green',
        cursor: 'pointer',
        color: 'white',
    };

    const inactive = {
        backgroundColor: 'gray',
        cursor: 'not-allowed',
        color: 'white',
    };

    return (
        <aside className={"border-2 flex flex-col"} style={{gridArea: "sidebar"}}>
            <div>
                MultiRoom
            </div>

            <nav className="my-8">
                <ul className="flex flex-col gap-8">
                    {navigationItems.map(item => (
                        <li key={item.name} className="text-center">
                            <button key={item.page} onClick={() => onNavigate(item.page)}>
                                <span style={page == item.page ? active : inactive}>{item.name}</span>
                            </button>
                        </li>
                    ))}
                </ul>
            </nav>
            <div className="flex-grow w-full">
            </div>
            <div>
                <label htmlFor="host">Host</label>
                <input className="border-2 w-full" id="host" type="text" value={host}
                       onChange={onChangeHost}/>
                <label htmlFor="port">Port</label>
                <input className="border-2 w-full" id="port" type="number" value={port}
                       onChange={onChangePort}/>
            </div>
            <div>
                <h3>SNAPCAST</h3>
                <div className="border-2">
                    {snapcastConnected ? <Connected/> : <Disconnected/>}
                </div>
            </div>
            <div>
                <h3>CONTROL</h3>
                <div className="border-2">
                    {websocketConnect ? <Connected/> : <Disconnected/>}
                </div>
            </div>

        </aside>
    )
}