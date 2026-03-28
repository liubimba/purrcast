import {useConfiguration} from "../../../hooks/useConfiguration.ts";
import type {ChangeEvent} from "react";
import * as React from "react";
import {ConnectionState, type ConnectionStatus,} from "../../../shared/client/entity/status.ts";

interface ConnectionDisplayUnit {
    title: string,
    status: ConnectionStatus
}

const FancyForm: React.FC<{
    type: string,
    value: string,
    title: string,
    onChange: (e: ChangeEvent<HTMLInputElement>) => void,
    revert?: boolean,
    className?: string,
}> = ({type, title, value, onChange, revert = false, className = ""}) => {
    return (
        <div
            className={`before:absolute 
            ${revert ? "before:-scale-x-100" : ""}
            before:[content:""]
            before:inset-0
            before:[mask-size:100%_100%] before:[-webkit-mask-size:100%_100%]
            before:bg-shadow
            before:pointer-events-none
            before:mask-no-repeat before:[-webkit-mask-repeat:no-repeat]
            before:[mask-image: url("/public/connectionsBoard/svg/server_board_background_hostport.svg")] before:[-webkit-mask-image:url("/public/connectionsBoard/svg/server_board_background_hostport.svg")]
            relative flex flex-col px-4 gap-4  ${className} ${revert ? "items-end" : ""}`}>
            <input type={type} value={value} onChange={onChange} id={`fancyForm-${title}`}
                   className="border-b-2 w-full pt-4 font-black "/>
            <label className="font-black pb-2" htmlFor={`fancyForm-${title}`}>{title}</label>
        </div>
    )
}

const ConnectionDisplay: React.FC<{ connections: ConnectionDisplayUnit[] }> = ({connections}) => {
    const mapToBackgroundColor = (connectionStatus: ConnectionStatus) => {
        if (connectionStatus.state == ConnectionState.CONNECTED) {
            return "--color-success";
        }
        if (connectionStatus.state == ConnectionState.FAILED) {
            return "--color-error";
        }
        return "--color-info";
    }
    return (
        <div className="border-3 flex justify-around">
            {connections.map((connection, index) => (
                <div key={`${connection.title}-${index}`} className="py-4 flex flex-col justify-center items-center">
                    <span className="font-black">{connection.title}</span>
                    <div className={`h-[12px] border-2 w-[80px]`}
                         style={{background: `var(${mapToBackgroundColor(connection.status)})`}}/>
                </div>
            ))}
        </div>
    )
}

export const ConnectionsBoard = () => {
    const {host, connect, connections} = useConfiguration();

    const units: ConnectionDisplayUnit[] = Object.entries(connections).map(([key, connectionStatus]) => ({
        title: key,
        status: connectionStatus
    } as ConnectionDisplayUnit))

    const onChangedHost = (address: string | null, port: number | null) => {
        console.log("TUTA", address, port, " -- ", address ? address : host.address, port ? port : host.port)
        connect(address ? address : host.address, port ? port : host.port);
    }

    return (
        <div className="px-4 gap-4 flex flex-col items-center h-full justify-center">
            <div className="flex flex-row gap-4 w-full">
                <FancyForm type="text" className="flex-1" title={"host"}
                           onChange={(e) => onChangedHost(e.target.value, null)}
                           value={host.address}/>
                <FancyForm type="number" className="flex-1" title={"port"}
                           onChange={(e) => onChangedHost(null, Number(e.target.value))}
                           value={host.port.toString()} revert={true}/>
            </div>
            <div className="w-full">
                <ConnectionDisplay connections={units}/>
            </div>
        </div>
    )
}