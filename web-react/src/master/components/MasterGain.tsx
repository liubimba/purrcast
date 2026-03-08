import * as React from "react";
import '../styles/masterGain.css'

interface GainProps {
    gain: number;
}

export const MasterGain: React.FC<GainProps> = ({gain}: GainProps) => {
    const segmentsCount: number = 24;
    const calculateBarColor = (index: number) => {
        if (index == segmentsCount - 1) {
            return "red";
        }
        return 100 * index / segmentsCount < gain ? "#FFFFFF" : "#4B4B4B";
    }

    return (
        <div className="master-gain-wrapper px-2 py-4">
            <div className="flex flex-col-reverse h-full gap-y-0.5">
                {Array.from(Array(segmentsCount).keys()).map((_, i) => (
                    <div key={i} className="flex flex-row flex-1 gap-2">
                        {Array.from(Array(2).keys()).map((_, j) => (
                            <div key={j} className="flex-1 w-full h-full" style={{
                                backgroundColor: calculateBarColor(i),
                            }}/>
                        ))}
                    </div>
                ))}
            </div>

        </div>
    )
}