import * as React from "react";

interface MuteButtonProps {
    muted: boolean;
    onToggle: () => void;
}

export const MuteButton: React.FC<MuteButtonProps> = ({muted, onToggle}) => {
    return (
        <button onClick={onToggle}>
            {muted ? (
                <>
                    <span>🗣️</span>
                </>
            ) : (
                <>
                    <span>🔇</span>
                </>
            )}
        </button>
    )
}