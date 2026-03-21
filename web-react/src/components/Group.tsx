import {Snapcast} from "../snapcontrol.ts";
import {useSnapcast} from "../hooks/useSnapcast.ts";
import {useSelector} from "react-redux";
import {selectClientsByGroupId} from "../store/selectors/snapcastSelector.ts";
import {Player} from "../player/components/Player.tsx";

interface RoomProps {
    group: Snapcast.Group;
    showOffline: boolean;
}


export const Group = (props: RoomProps) => {
    const {setClientMuted, setClientVolume} = useSnapcast();

    const clients: Snapcast.Client[] | undefined = useSelector(selectClientsByGroupId(props.group.id));

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