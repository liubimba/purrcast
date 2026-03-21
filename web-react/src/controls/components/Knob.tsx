import {useEffect, useRef, useState} from "react";
import '../styles/knob.css'

interface KnobProps {
    volume?: number;
    onChange?: (value: number) => void;
}

class Angle {
    private _angle: number;
    private readonly _minAngle: number;
    private readonly _maxAngle: number;
    private _isDragging: boolean = false;
    private _lastClientY: number | null = null;

    constructor(min: number, max: number, volume: number) {
        this._minAngle = min;
        this._maxAngle = max;
        this._angle = min + volume * (max - min);
    }

    public drag() {
        this._isDragging = true;
    }

    public release() {
        this._isDragging = false;
        this._lastClientY = null;
    }

    public get isDragging(): boolean {
        return this._isDragging;
    }

    public moved(clientY: number): number {
        if (!this._isDragging) return this.volume;

        if (this._lastClientY === null) {
            this._lastClientY = clientY;
            return this.volume;
        }

        const delta = (this._lastClientY - clientY) / 200;
        this._lastClientY = clientY;
        const angle: number = Math.min(this._maxAngle, Math.max(this._minAngle, this._angle + delta * (this._maxAngle - this._minAngle)));
        return this.getVolume(angle);
    }

    public get degree(): number {
        return this._angle;
    }

    public getVolume(angle: number): number {
        return Math.round((angle - this._minAngle) / (this._maxAngle - this._minAngle) * 100);
    }

    public get volume(): number {
        return Math.round((this._angle - this._minAngle) / (this._maxAngle - this._minAngle) * 100);
    }

    public set value(v: number) {
        this._angle = this._minAngle + v * (this._maxAngle - this._minAngle);
    }
}

export const Knob: React.FC<KnobProps> = ({volume = 50, onChange}) => {
    const angleRef = useRef<Angle>(new Angle(-135, 135, volume));
    const volumeRef = useRef<number>(volume);

    const [rotation, setRotation] = useState(() => angleRef.current.degree);

    useEffect(() => {
        volumeRef.current = volume;
        angleRef.current.value = volume / 100;

        setRotation(angleRef.current.degree);
    }, [volume]);

    const onMouseDown = (): void => {
        angleRef.current.drag();
    };

    const onMouseMove = (e: MouseEvent): void => {
        if (angleRef.current.isDragging) {
            const movedVolume: number = angleRef.current.moved(e.clientY);
            if (movedVolume !== volumeRef.current) {
                onChange?.(movedVolume);
            }
        }
    };

    const onMouseUp = (): void => {
        angleRef.current.release();
    };

    useEffect(() => {
        window.addEventListener("mouseup", onMouseUp);
        window.addEventListener("mousemove", onMouseMove);
        return () => {
            window.removeEventListener("mouseup", onMouseUp);
            window.removeEventListener("mousemove", onMouseMove);
        }
    }, []);


    return (
        <div
            className="knob-container"
            style={{rotate: `${rotation}deg`}}
            onMouseDown={onMouseDown}
        >
            <div className="w-full h-full -rotate-[10deg]">
                <div className="knob-arc-line"/>
                <div className="knob-arc"/>
            </div>

            <div className="knob-circle"/>
            <div className="knob-dot"/>
            <div className="knob-line"/>
        </div>
    );
};