import {memo} from "react";

interface AppLayoutProps {
    sidebar: React.ReactNode;
    content: React.ReactNode;
    playerbar: React.ReactNode;
}


export const AppLayout = memo(function AppLayout({sidebar, content, playerbar}: AppLayoutProps) {
    return (
        <div className="app-layout">
            <aside className="app-sidebar glass glass-intensity-high">{sidebar}</aside>
            <main className="app-content glass glass-intensity-high">{content}</main>
            <footer className="app-playerbar  glass glass-intensity-high">{playerbar}</footer>
        </div>
    )
});