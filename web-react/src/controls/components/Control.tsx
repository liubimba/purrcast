import '../styles/control.css'
import * as React from "react";
import {useEffect, useState} from "react";
import {VolumeBar} from "./VolumeBar.tsx";
import {Knob} from "./Knob.tsx";
import {Toggle} from "./Toggle.tsx";

interface ControlProps {
    volume: number,
    muted: boolean,
    name: string,
    index: number,
    onVolumeChange?: (volume: number) => void,
    onMutedChange?: (muted: boolean) => void,
}

export const Control: React.FC<ControlProps> = ({
                                                    volume,
                                                    muted,
                                                    name,
                                                    index,
                                                    onVolumeChange,
                                                    onMutedChange
                                                }: ControlProps) => {
    return (
        <div className="control-container flex-col flex app-border">
            <div className="ml-24 mr-4 control-header mb-4">
                <div className="flex gap-4 justify-between mb-8">
                    <div>
                        <span style={{fontSize: "0.8rem"}} className="font-black">PLAYER</span>
                    </div>
                    <div>
                        <span style={{fontSize: "0.8rem"}} className="font-black">STEREO</span>
                    </div>
                </div>


                <div className="mb-1">
                    <span style={{fontSize: "24px"}} className="font-black">{volume}/100</span>
                </div>

                <VolumeBar bars={80} volume={volume} width={660} height={48}/>
            </div>

            <div className="flex-1 flex flex-wrap">
                <div className="border-t-1 border-r-1  flex-2 px-8 py-4">
                    <div>
                        <span style={{fontSize: "0.8rem"}} className="font-black">CONTROLLER</span>
                    </div>
                    <div className="flex flex-wrap-reverse  justify-center items-center gap-12">
                        <Toggle muted={muted} onToggle={() => onMutedChange?.(!muted)}/>

                        <div style={{minHeight: 200, minWidth: 200}}>
                            <Knob volume={volume} onChange={(volume: number) => {
                                onVolumeChange?.(volume);
                            }}/>
                        </div>
                    </div>
                </div>
                <div className="flex-1 flex flex-col justify-between px-8 py-4 border-t-1">
                    <div className="">
                        <span style={{fontSize: "0.8rem"}} className="font-black">ABOUT</span>
                    </div>
                    <div className="">
                        <h2 style={{fontSize: "1.5rem", fontWeight: 900}}>{String(index).padStart(2, '0')}</h2>
                        <h2 style={{fontSize: "2rem", fontWeight: 900}}>{name}</h2>
                    </div>
                </div>
            </div>

        </div>
    )
}

export const TestControl = () => {
    const [volume, setVolume] = useState(32);
    const [muted, setMuted] = useState(true);

    useEffect(() => {
        let increasing = true;
        const interval = setInterval(() => {
            setVolume(prev => {
                if (prev >= 100) increasing = false;
                if (prev <= 0) increasing = true;
                return increasing ? prev + 1 : prev - 1;
            });
        }, 20); // 100 шагов за 2 секунды = 20ms на шаг

        return () => clearInterval(interval);
    }, []);

    return (
        <Control volume={volume} muted={muted} name={"test"} index={1} onVolumeChange={setVolume}
                 onMutedChange={setMuted}/>
    )
}