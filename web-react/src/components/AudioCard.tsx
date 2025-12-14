import {useEffect, useState} from "react";
import {Snapcast, SnapControl} from "../snapcontrol.ts";
import {VolumeSlider} from "./VolumeSlider.tsx";
import {MuteButton} from "./MuteButton.tsx";

interface Props {
    client: Snapcast.Client;
    snapcontrol: SnapControl
}

export const AudioCard: React.FC<Props> = ({client, snapcontrol}) => {
    const [name, setName] = useState(client.config.name);
    const [muted, setMuted] = useState(client.config.volume.muted);
    const [volume, setVolume] = useState(client.config.volume.percent);
    const [latency, setLatency] = useState(client.config.latency);

    useEffect(() => {
        setName(client.config.name);
    }, [client.config.name]);

    useEffect(() => {
        setVolume(client.config.volume.percent);
    }, [client.config.volume.percent]);

    useEffect(() => {
        setLatency(client.config.latency);
    }, [client.config.latency]);

    useEffect(() => {
        setMuted(client.config.volume.muted);
    }, [client.config.volume.muted]);

    const onVolumeChange = (volume: number) => {
        setVolume(volume);
        snapcontrol.setVolume(client.id, volume, muted);
    };

    const onMutedToggle = () => {
        setMuted(!muted);
        snapcontrol.setVolume(client.id, client.config.volume.percent, muted);
    }

    const onLatencyChanged = (latency: number) => {
        setLatency(latency);
        snapcontrol.setClientLatency(client.id, latency);
    }

    const onNameChanged = (name: string) => {
        setName(name);
        snapcontrol.setClientName(client.id, name);
    }

    return (

        <div className="border-2 border-gray-200">
            <MuteButton muted={muted} onToggle={onMutedToggle}/>


            <label htmlFor="latency">
                Latency
            </label>
            <input type="number"
                   className="border-2 "
                   id={`${client.id}-latency`}
                   value={latency}
                   name="latency"
                   onChange={(event) =>
                       onLatencyChanged(Number(event.target.value))
                   }/>


            <label htmlFor={`${client.id}-name`}>
                Name
            </label>
            <input type="string"
                   className="border-2 "
                   id={`${client.id}-name`}
                   value={name}
                   name="name"
                   onChange={(event) =>
                       onNameChanged(event.target.value)
                   }/>

            <VolumeSlider volume={volume} onChange={onVolumeChange}/>
        </div>

    );
};
