import * as React from "react";
import {MasterGain} from "./MasterGain.tsx";
import {MasterVolumeBar} from "./MasterVolumeBar.tsx";
import {Toggle} from "../../controls/components/Toggle.tsx";
import {useMasterPlayer} from "../hooks/useMasterPlayer.ts";

export const MasterPlayer: React.FC<{}> = () => {
    const {masterVolume, masterMuted, updateMasterVolume, updateMasterMuted} = useMasterPlayer();

    return (
        <div className="flex gap-24  align-center justify-center h-full">
            <MasterGain gain={0}/>
            <div className="flex flex-1 flex-wrap gap-12">
                <MasterVolumeBar className="flex-1" volume={masterVolume} onVolumeChanged={updateMasterVolume}/>
                <Toggle muted={masterMuted} onToggle={() => {
                    updateMasterMuted(!masterMuted);
                }}/>
            </div>
        </div>
    )
}
