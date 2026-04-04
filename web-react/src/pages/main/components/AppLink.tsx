import {useConfiguration} from "../../../hooks/useConfiguration.ts";
import {useElectron} from "../../../hooks/useElectron.ts";
import * as React from "react";
import {ConnectionState} from "../../../shared/client/entity/status.ts";

const Wrapper: React.FC<{ children: React.ReactNode }> = ({children}) => {
    return (
        <div className="z-99 flex justify-center gap-4 w-full bg-shadow text-primary font-semibold  p-4">
            {children}
        </div>
    )
}


const Link: React.FC<{ address: string, port: number }> = ({address, port}) => {
    const link: string = `http://${address}:${port}`;
    return (
        <a target="_blank" href={link} className="text-link">{link}</a>
    )
}

export const AppLink = () => {
    const isElectron = useElectron();
    const {host, connections} = useConfiguration();

    if (!isElectron || connections.configuration.state !== ConnectionState.CONNECTED) {
        return (
            <></>
        )
    }

    if (host.hostname) {
        return (
            <Wrapper>
                <span>this app available on browser</span>
                <Link address={host.hostname} port={host.port}/>
                <span>or if its not working</span>
                <Link address={host.address} port={host.port}/>
            </Wrapper>
        )
    }

    return (
        <Wrapper>
            <span>this app available on browser</span>
            <Link address={host.address} port={host.port}/>
        </Wrapper>
    )
}