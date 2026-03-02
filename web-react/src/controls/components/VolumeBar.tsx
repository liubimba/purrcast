import * as React from "react";
import {useEffect, useRef} from "react";

interface VolumeBarProps {
    volume: number,
    bars: number,
    width: number,
    height: number,
}

export const VolumeBar: React.FC<VolumeBarProps> = ({volume, height}: VolumeBarProps) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);

    useEffect(() => {
        const canvas = canvasRef.current;
        if (!canvas) {
            return;
        }
        const ctx = canvas.getContext("2d")!;
        const width = canvas.width;
        const height = canvas.height;
        const bars = width / 2;
        ctx.clearRect(0, 0, width, height);
        const barWidth = width / bars;
        const gap = barWidth * 0.6;
        const filled = Math.round(volume / 100 * bars);

        for (let i = 0; i < bars; i++) {
            const x = i * barWidth;
            const active = i < filled;

            ctx.fillStyle = active ? "#1a1a1a" : "#b0a898";
            ctx.fillRect(x, 0, barWidth - gap, height);
        }
    }, [volume]);

    return (
        <div>
            <canvas className="w-full" ref={canvasRef} height={height}/>
        </div>
    )
}
