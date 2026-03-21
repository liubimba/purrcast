import type {ControlProps} from "../entity/ControlProps.ts";
import * as React from "react";
import {Control} from "./Control.tsx";
import {useSnapcast} from "../../hooks/useSnapcast.ts";


export const Controls: React.FC<{}> = () => {
    const {snapserver, setClientVolume, setClientMuted} = useSnapcast();

    const controls: ControlProps[] = [];
    snapserver.groups.forEach(group => {
        group.clients.forEach(client => {
            if (!client.connected) {
                return;
            }
            const props: ControlProps = {
                muted: client.config.volume.muted,
                volume: client.config.volume.percent,
                name: client.host.name,
                index: 0,
                onMutedChange: (muted) => {
                    setClientMuted(client.id, muted);
                },
                onVolumeChange: (volume) => {
                    setClientVolume(client.id, volume);
                }
            }
            controls.push(props);
        })
    })

    return (
        <div className="flex flex-wrap gap-8">
            {controls.map((control: ControlProps, inex) => (
                <Control volume={control.volume}
                         muted={control.muted}
                         name={control.name}
                         className={control.className}
                         key={`${control.name}-${inex}`}
                         index={control.index}
                         onMutedChange={control.onMutedChange}
                         onVolumeChange={control.onVolumeChange}/>
            ))}
        </div>
    )
}