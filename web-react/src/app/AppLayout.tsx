import {memo, useRef} from "react";

interface AppLayoutProps {
    header: React.ReactNode;
    content: React.ReactNode;
    playerbar: React.ReactNode;
    info: React.ReactNode;
}


export const AppLayout = memo(function AppLayout({header, content, playerbar, info}: AppLayoutProps) {
    const containerRef = useRef<HTMLDivElement>(null);

    return (
        <div className="app-layout" ref={containerRef}>
            <header className="app-header app-wrapper  app-border">{header}</header>
            <main className="app-content app-wrapper  app-border">
                <h2 className="app-title">CONTROLS</h2>
                {content}
            </main>
            <div className="app-info app-wrapper app-border">
                <h2 className="app-title text-center">INFO</h2>
                {info}
            </div>
            <footer className="app-playerbar app-wrapper  app-border">
                <h2 className="app-title">MASTER</h2>
                {playerbar}
            </footer>
        </div>
    )
});