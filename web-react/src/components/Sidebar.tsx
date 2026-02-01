import * as React from "react";
import {Logger} from "../shared/logger/logger.ts";
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
        {name: "Monitor", icon: undefined, page: "monitor"},
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


    return (
        <section className={"flex flex-col justify-center items-center h-full w-full"}>
            <nav className="">
                <ul className="flex items-center flex-col gap-8">
                    {navigationItems.map(item => (
                        <li key={item.name} className="text-center">
                            <button className={`btn ${item.page === page ? "active" : ""}`}
                                    key={item.page} onClick={() => onNavigate(item.page)}>
                                <span> {item.name}</span>
                            </button>
                        </li>
                    ))}
                </ul>
            </nav>

            <div>
                <label htmlFor="host">Host</label>
                <input className="w-full" id="host" type="text" value={host}
                       onChange={onChangeHost}/>
                <label htmlFor="port">Port</label>
                <input className=" w-full" id="port" type="number" value={port}
                       onChange={onChangePort}/>
            </div>
            <div>
                <h3>SNAPCAST</h3>
                <div className="">
                    {snapcastConnected ? <Connected/> : <Disconnected/>}
                </div>
            </div>
            <div>
                <h3>CONTROL</h3>
                <div className="">
                    {websocketConnect ? <Connected/> : <Disconnected/>}
                </div>
            </div>

        </section>
    )
}