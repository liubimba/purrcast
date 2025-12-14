import * as React from "react";
import {Snapcast} from "../snapcontrol.ts";
import {Group} from "../components/Group.tsx";
import {useSnapcast} from "../hooks/useSnapcast.ts";

interface MainProps {

}


export const Main: React.FC<MainProps> = ({}) => {
    const {snapserver} = useSnapcast();

    return (
        <div>
            {snapserver.groups.map((group: Snapcast.Group) => (
                <Group key={group.id} group={group} showOffline={false}/>
            ))}
        </div>

    );
}

