import * as React from "react";
import {type ChangeEvent, useEffect, useRef} from "react";
import '../styles/masterVolumeBar.css'

interface MasterVolumeBarProps {
    volume: number;
    onVolumeChanged: (volume: number) => void;
    className?: string;
}

export const MasterVolumeBar: React.FC<MasterVolumeBarProps> = ({
                                                                    volume,
                                                                    onVolumeChanged,
                                                                    className = ""
                                                                }: MasterVolumeBarProps) => {
    const sliderThumbRef = useRef<HTMLDivElement>(null);

    const onInputChange = (event: ChangeEvent<HTMLInputElement>) => {
        const value: number = Number(event.target.value);
        console.log(value);
        onVolumeChanged(value);
    }

    useEffect(() => {
        if (sliderThumbRef.current) {
            const sliderThumb: HTMLDivElement = sliderThumbRef.current as HTMLDivElement;
            sliderThumb.style.left = `${volume}%`;
        }
    }, [volume]);

    return (
        <div className={"gap-4 align-center justify-center " + className}>
            <div className="min-w-[160px] relative">
                <div className="absolute top-1/2 pointer-events-none w-full flex">
                    {Array.from(Array(10).keys()).map((_, __) => (
                        <div className="flex-1" key={__}>
                            <div className="border-1 w-0 h-[12px]"/>
                        </div>
                    ))}
                </div>
                <input type="range" min="0" max="100" value={volume}
                       className=" bg-transparent [-webkit-appearance:none] w-full h-[80px]
                       [&::-moz-range-thumb]:cursor-col-resize
                       [&::-webkit-slider-thumb]:cursor-col-resize
                       [&::-webkit-slider-runnable-track]:h-1/2
                       [&::-webkit-slider-runnable-track]:rounded-full
                       [&::-webkit-slider-runnable-track]:border-2
                       [&::-webkit-slider-runnable-track]:border-black
                       [&::-webkit-slider-runnable-track]:shadow-[-4px_4px_0_#000]"
                       onChange={onInputChange}/>
                <div ref={sliderThumbRef} className="slider-thumb">
                    <div className="border-1 w-0 flex-1"/>
                    <span> {volume}</span>
                    <div className="border-1 w-0 flex-1"/>
                </div>
            </div>

            <div className="hidden md:flex justify-between">
                {Array.from(Array(11).keys()).map((index, __) =>
                    <div key={index} className="relative bg-black w-[1px] h-[50px]">
                        <div className="absolute bg-black top-1/2 -translate-y-1/2 left-[-10px] w-[1px] h-[30px]"/>
                        <div className="absolute bg-black top-1/2 -translate-y-1/2 left-[10px] w-[1px] h-[30px]"/>
                    </div>
                )}
            </div>

            <div className="hidden md:flex justify-between">
                {Array.from(Array(11).keys()).map((index, __) =>
                    <div className="w-0">
                        <div className=" max-w-fit min-w-fit -translate-x-1/2">
                                                    <span
                                                        className="font-black text-center text-[clamp(8px,0.8cqi,2cqi)]">{index * 10}</span>
                        </div>

                    </div>
                )}
            </div>
        </div>
    )
}