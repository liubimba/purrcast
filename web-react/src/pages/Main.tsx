import * as React from "react";
import {Snapcast} from "../snapcontrol.ts";
import {Group} from "../components/Group.tsx";
import {useSnapcast} from "../hooks/useSnapcast.ts";
import {TitleWrapper} from "../components/title/TitleWrapper.tsx";

interface MainProps {

}


export const Main: React.FC<MainProps> = ({}) => {
    const {snapserver} = useSnapcast();

    return (
        <TitleWrapper title="Controls" wrapperClassName="w-full">
            <div className="flex  justify-center gap-16 pt-16">
                {snapserver.groups.map((group: Snapcast.Group) => (
                    <Group key={group.id} group={group} showOffline={false}/>
                ))}
            </div>
        </TitleWrapper>
    );
}




























