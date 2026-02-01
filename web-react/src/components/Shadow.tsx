import {memo, type ReactNode} from "react";

type ShadowProps = {
    children: ReactNode;
    className?: string;
}

export const Shadow = memo(function Shadow({children, className = ""}: ShadowProps) {
    return (
        <div className={`${className} shadow`}>
            {children}
        </div>
    )
})