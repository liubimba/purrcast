import '../styles/control.css'
import * as React from "react";
import {VolumeBar} from "./VolumeBar.tsx";
import {Knob} from "./Knob.tsx";
import {Toggle} from "./Toggle.tsx";
import type {ControlProps} from "../entity/ControlProps.ts";


export const Control: React.FC<ControlProps> = ({
                                                    volume,
                                                    muted,
                                                    name,
                                                    index,
                                                    onVolumeChange,
                                                    onMutedChange,
                                                    className = ""
                                                }: ControlProps) => {
    return (
        <div className={"flex-col flex app-border overflow-hidden  flex-1 min-w-[400px] " + className}>
            <div className="ml-24 mr-4 control-header mb-4">
                <div className="flex gap-4 justify-between mb-8">
                    <div>
                        <span className="text-[0.8rem] font-black">PLAYER</span>
                    </div>
                    <div>
                        <span className="text-[0.8rem] font-black">STEREO</span>
                    </div>
                </div>


                <div className="mb-1">
                    <span className="font-black text-[1.5rem]">{volume}/100</span>
                </div>

                <VolumeBar bars={80} volume={volume} width={660} height={48}/>
            </div>

            <div className="flex-1 flex flex-wrap">
                <div className="border-t-1 border-r-1  flex-2 px-8 py-4">
                    <div>
                        <span className="font-black text-[0.8rem]">CONTROLLER</span>
                    </div>
                    <div className="flex flex-wrap-reverse  justify-center items-center gap-12">
                        <Toggle muted={muted} onToggle={() => onMutedChange?.(!muted)}/>

                        <div className="aspect-square min-h-[200px]">
                            <Knob volume={volume} onChange={(volume: number) => {
                                onVolumeChange?.(volume);
                            }}/>
                        </div>
                    </div>
                </div>
                <div className="flex-1 flex flex-col justify-between px-8 py-4 border-t-1">
                    <div className="">
                        <span className="text-[0.8rem] font-black">ABOUT</span>
                    </div>
                    <div className="">
                        <h2 className="font-black text-[1.5rem]">{String(index).padStart(2, '0')}</h2>
                        <h2 className="font-black text-[2rem]">{name}</h2>
                    </div>
                </div>
            </div>

        </div>
    )
}
