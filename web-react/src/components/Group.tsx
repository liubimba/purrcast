import {Snapcast} from "../snapcontrol.ts";
import * as React from "react";
import {useSnapcast} from "../hooks/useSnapcast.ts";
import {useSelector} from "react-redux";
import {selectClientsByGroupId, selectStreamById} from "../store/selectors/snapcastSelector.ts";
import {Player} from "../player/components/Player.tsx";

interface RoomProps {
    group: Snapcast.Group;
    showOffline: boolean;
}


const MuteButton: React.FC<{ muted: boolean, onMutedToggle: () => void }> = ({muted, onMutedToggle}) => {
    return (
        <>
            <button onClick={onMutedToggle}>{muted ? "Muted" : "Unmuted"}</button>
        </>
    )
}

const VolumeSlider: React.FC<{ volume: number, onVolumeChanged: (volume: number) => void }> = ({
                                                                                                   volume,
                                                                                                   onVolumeChanged
                                                                                               }) => {
    return (
        <>
            <input type="range"
                   min="0" max="100" value={volume}
                   onChange={(event: React.ChangeEvent<HTMLInputElement>) => onVolumeChanged(Number(event.target.value))}/>
        </>
    )
}

export const Group = (props: RoomProps) => {
    const {setClientMuted, setClientVolume} = useSnapcast();

    const clients: Snapcast.Client[] | undefined = useSelector(selectClientsByGroupId(props.group.id));
    const stream: Snapcast.Stream | undefined = useSelector(selectStreamById(props.group.stream_id));

    if (!clients || clients.length == 0) {
        return <></>;
    }

    console.log(clients);
    return (
        <div className="">
            {clients.map((client) => (
                <Player key={client.id}
                        name={client.host.name}
                        muted={client.config.volume.muted}
                        volume={client.config.volume.percent}
                        onVolumeChanged={(volume: number) => {
                            setClientVolume(client.id, volume);
                        }}
                        onMutedToggle={() => {
                            setClientMuted(client.id, !client.config.volume.muted);
                        }}/>
            ))}
        </div>
    )
}