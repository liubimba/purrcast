import * as React from "react";
import {type ChangeEvent, useEffect, useRef} from "react";
import '../styles/masterVolumeBar.css'

interface MasterVolumeBarProps {
    volume: number;
    onVolumeChanged: (volume: number) => void;
}

export const MasterVolumeBar: React.FC<MasterVolumeBarProps> = ({volume, onVolumeChanged}: MasterVolumeBarProps) => {
    const sliderThumbRef = useRef<HTMLDivElement>(null);

    const onInputChange = (event: ChangeEvent<HTMLInputElement>) => {
        const value: number = Number(event.target.value);
        onVolumeChanged(value);
    }

    useEffect(() => {
        if (sliderThumbRef.current) {
            const sliderThumb: HTMLDivElement = sliderThumbRef.current as HTMLDivElement;
            sliderThumb.style.left = `${volume}%`;
        }
    }, [volume]);

    return (
        <div className="flex flex-col gap-4 flex-1 align-center justify-center">
            <div className="slider-wrapper">
                <div className="absolute pointer-events-none w-full h-full flex ">
                    {Array.from(Array(10).keys()).map((_, __) => (
                        <div className="flex-1 flex items-center" key={__}>
                            <div className="border-1 w-0" style={{height: "12px"}}/>
                        </div>
                    ))}
                </div>
                <input type="range" min="0" max="100" value={volume} className="slider" id="slider"
                       onChange={onInputChange}/>
                <div ref={sliderThumbRef} className="slider-thumb">
                    <div className="border-1 w-0 flex-1"/>
                    <span> {volume}</span>
                    <div className="border-1 w-0 flex-1"/>
                </div>
            </div>
            <div className="flex justify-between slider-gain">
                {Array.from(Array(11).keys()).map((index, __) =>
                    <div key={`master-slider-notch-${index}`} className="flex flex-col bg-black ">
                        <div className="relative" style={{
                            width: "1px",
                            height: "50px"
                        }}>
                            <div className="absolute bg-black top-1/2 -translate-y-1/2" style={{
                                left: "-10px",
                                width: "1px",
                                height: "30px",
                            }}/>

                            <div className="absolute bg-black top-1/2 -translate-y-1/2" style={{
                                left: "10px",
                                width: "1px",
                                height: "30px",
                            }}/>
                            <div className="absolute top-full -translate-x-1/2">
                                    <span className="gap-4 flex-1" style={{
                                        fontWeight: 900,
                                    }}>{(index) * 10}</span>
                            </div>
                        </div>
                    </div>)}
            </div>
        </div>
    )
}