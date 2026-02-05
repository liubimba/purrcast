import * as React from "react";
import '../styles/player.css'
import '../assets/icons/mute.png'
import '../assets/icons/sound.png'

interface PlayerProps {
    muted: boolean;
    volume: number;
    name: string,
    onVolumeChanged: (volume: number) => void;
    onMutedToggle: () => void;
}

export const Player: React.FC<PlayerProps> = ({muted, volume, name, onVolumeChanged, onMutedToggle}: PlayerProps) => {

    return (
        <div className=" flex flex-col  items-center ">
            <div className="player-header">
                <h2>{name}</h2>
            </div>
            <div className="player-slider">
                <div
                    className="player-fill "
                    style={{height: `${volume * 100}%`}}
                />

                <input
                    type="range"
                    min={0}
                    max={1}
                    step={0.01}
                    value={volume}
                    onChange={(e) => onVolumeChanged(Number(e.target.value))}
                    className="player-input "
                />
            </div>

            <button onClick={onMutedToggle}>
                <div
                    className={`player-button ${muted ? "player-button-muted" : "player-button-sound"}`}></div>
            </button>
        </div>
    )
}