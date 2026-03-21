import * as React from "react";
import {MasterGain} from "./MasterGain.tsx";
import {MasterVolumeBar} from "./MasterVolumeBar.tsx";
import {Toggle} from "../../controls/components/Toggle.tsx";
import {useMasterPlayer} from "../../hooks/useMasterPlayer.ts";

export const MasterPlayer: React.FC<{}> = () => {
    const {masterVolume, masterMuted, updateMasterVolume, updateMasterMuted} = useMasterPlayer();

    const audioContext = new AudioContext();
    const analyser = audioContext.createAnalyser();
    analyser.connect(audioContext.destination);
    analyser.fftSize = 256;
    const data = new Uint8Array(analyser.frequencyBinCount);

    function getVolume() {
        analyser.getByteTimeDomainData(data);

        let sum = 0;

        for (let i = 0; i < data.length; i++) {
            const v = (data[i] - 128) / 128;
            sum += v * v;
        }

        const rms = Math.sqrt(sum / data.length);

        console.log("volume", rms);

        requestAnimationFrame(getVolume);
    }

    getVolume();

    return (
        <div className="flex gap-24  align-center justify-center">
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
