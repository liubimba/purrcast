import * as React from "react";
import '../styles/toggle.css'

export const Toggle: React.FC<{ muted: boolean, onToggle: () => void, height?: number }> = ({muted, onToggle}) => {
    return (
        <div className="toggle-container" onClick={(_) => onToggle()}>
            <div className="toggle-header" style={{boxShadow: `${!muted ? "-4px" : "4px"} 0px 0 black`}}>
                <div className={`${muted ? "toggle-rhombus" : "toggle-line"}`}>

                </div>
                <div className={`${!muted ? "toggle-rhombus" : "toggle-line"}`}>

                </div>
            </div>
            <div className="flex flex-row toggle-footer">
                <div>
                    <span className="toggle-text">Off</span>
                </div>
                <div>
                    <span className="toggle-text">On</span>
                </div>
            </div>
        </div>

    )
}