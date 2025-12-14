import * as React from "react";

interface IconButtonProps {
    onClick: () => void;
    icon: React.ElementType;
}

export const IconButton: React.FC<IconButtonProps> = ({onClick, icon: Icon}) => {
    return (
        <button onClick={onClick}>
            <Icon/>
        </button>
    )
}