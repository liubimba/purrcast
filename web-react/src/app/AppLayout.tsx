import {memo, useRef} from "react";

interface AppLayoutProps {
    sidebar: React.ReactNode;
    content: React.ReactNode;
    playerbar: React.ReactNode;
}


export const AppLayout = memo(function AppLayout({sidebar, content, playerbar}: AppLayoutProps) {
    const containerRef = useRef<HTMLDivElement>(null);

    return (
        <div className="app-layout" ref={containerRef}>
            <aside className="app-sidebar glass glass-intensity-high">{sidebar}</aside>
            <main className="app-content  glass-intensity-high flex justify-center items-center">{content}</main>
            <footer className="app-playerbar  glass glass-intensity-high">{playerbar}</footer>
        </div>
    )
});