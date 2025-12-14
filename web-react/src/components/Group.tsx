import {Snapcast} from "../snapcontrol.ts";
import * as React from "react";
import {useSnapcast} from "../hooks/useSnapcast.ts";
import {useSelector} from "react-redux";
import {selectClientsByGroupId, selectStreamById} from "../store/selectors/snapcastSelector.ts";

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

    return (
        <div className="border-2">
            <div>
                {stream ? stream.id : "Undefined"}
            </div>
            {/*<StreamsList server={props.server} onSelectStream={onSelectedStream}/>*/}

            {clients.map((client) => (
                <div key={client.id} className="border-2 border-gray-200">
                    <MuteButton onMutedToggle={() => {
                        setClientMuted(client.id, !client.config.volume.muted);
                    }} muted={client.config.volume.muted}/>

                    <VolumeSlider volume={client.config.volume.percent} onVolumeChanged={(volume: number): void => {
                        setClientVolume(client.id, volume);
                    }}/>


                    {/*<label htmlFor="latency">*/}
                    {/*    Latency*/}
                    {/*</label>*/}
                    {/*<input type="number"*/}
                    {/*       className="border-2 "*/}
                    {/*       id={`${client.id}-latency`}*/}
                    {/*       value={latency}*/}
                    {/*       name="latency"*/}
                    {/*       onChange={(event) =>*/}
                    {/*           onLatencyChanged(Number(event.target.value))*/}
                    {/*       }/>*/}


                    {/*<label htmlFor={`${client.id}-name`}>*/}
                    {/*    Name*/}
                    {/*</label>*/}
                    {/*<input type="string"*/}
                    {/*       className="border-2 "*/}
                    {/*       id={`${client.id}-name`}*/}
                    {/*       value={name}*/}
                    {/*       name="name"*/}
                    {/*       onChange={(event) =>*/}
                    {/*           onNameChanged(event.target.value)*/}
                    {/*       }/>*/}

                    {/*<input type="range" min={0} max={100} value={props.volume}*/}
                    {/*       onChange={e => props.onChange(Number(e.target.value))}/>*/}
                </div>
            ))}
        </div>
    )
}