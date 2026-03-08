import * as React from "react";
import {MasterGain} from "./MasterGain.tsx";
import {MasterVolumeBar} from "./MasterVolumeBar.tsx";
import {Toggle} from "../../controls/components/Toggle.tsx";

interface MasterPlayerProps {
    gain: number;
    volume: number;
    muted: boolean;
    onVolumeChange: (volume: number) => void;
    onMutedChange: (muted: boolean) => void;
}

export const MasterPlayer: React.FC<MasterPlayerProps> = ({
                                                              gain,
                                                              muted,
                                                              volume,
                                                              onMutedChange,
                                                              onVolumeChange
                                                          }: MasterPlayerProps) => {
    return (
        <div className="flex gap-24 align-center justify-center h-full">
            <MasterGain gain={gain}/>
            <div className="flex flex-1 flex-wrap gap-12">
                <MasterVolumeBar volume={volume} onVolumeChanged={onVolumeChange}/>
                <Toggle muted={muted} onToggle={() => {
                    onMutedChange(!muted);
                }}/>
            </div>
        </div>
    )
}
