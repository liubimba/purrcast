import * as React from "react";
import {memo} from "react";
import clsx from "clsx";

type GlassProps = {
    children: React.ReactNode;
    intensity?: "low" | "medium" | "high";
    className?: string;
}

export const Glass = memo(function Glass({children, intensity = "high", className = ""}: GlassProps) {
    return (
        <div className={clsx("glass-root", `glass-intensity-${intensity} ${className}`)}>
            <div className="glass-effect" aria-hidden>
                <div className="glass-noise"/>
                <div className="glass-tint"/>
                <div className="glass-blur"/>
            </div>

            <div className="glass-content w-full h-full">
                {children}
            </div>
        </div>
    )
});