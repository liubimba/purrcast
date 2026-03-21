import * as React from "react";
import {memo} from "react";


export interface ContentProps {
    node: React.ReactNode;
    title: string;
}

interface AppLayoutProps {
    header: React.ReactNode;
    content: ContentProps;
    playerBar: ContentProps;
    info: ContentProps;
    serverBoard: ContentProps;
}

export const AppLayout = memo(function AppLayout({
                                                     header,
                                                     content,
                                                     playerBar,
                                                     info,
                                                     serverBoard
                                                 }: AppLayoutProps) {
    return (
        <div className="flex gap-8 flex-col h-screen bg-primary">
            <header>{header}</header>
            <div
                className="m-8 grid h-full gap-6 md:grid-cols-[1fr_auto] md:grid-rows-[1fr_auto] gap-6p-10 min-h-0 grid-cols-1 grid-rows-[auto_auto_auto]">
                <main
                    className="md:overflow-y-auto min-h-0 md:row-span-1 md:panel flex flex-col md:col-start-1 md:row-start-1">
                    <div className="md:sticky px-4 pt-4 top-0 z-10 backdrop-blur-3xl bg-transparent">
                        <h2 className="grid-title max-sm:text-center">
                            {content.title}
                        </h2>
                    </div>

                    <div className="px-4 max-sm:overflow-y-auto max-sm:panel  max-sm:p-4 pb-4">
                        {content.node}
                    </div>
                </main>

                <div
                    className="md:overflow-y-auto flex flex-col min-h-0 md:panel md:col-start-2 md:row-start1">
                    <div className="px-4 pt-4 md:sticky top-0 z-10 backdrop-blur-3xl">
                        <h2 className="text-center grid-title">
                            {info.title}
                        </h2>
                    </div>

                    <div className="max-sm:panel max-sm:overflow-y-auto pb-4">
                        {info.node}
                    </div>
                </div>

                <div className="md:panel md:row-start-2 md:col-start-2">
                    <div className="px-4 pt-4 md:sticky top-0 z-10 backdrop-blur-3xl">
                        <h2 className="text-center grid-title">
                            {serverBoard.title}
                        </h2>
                    </div>

                    <div className="">
                        {serverBoard.node}
                    </div>
                </div>

                <footer
                    className="flex flex-col md:panel p-4 md:col-start-1 md:row-start-2">
                    <h2 className="grid-title max-sm:text-center">
                        {playerBar.title}
                    </h2>
                    <div className="max-sm:panel max-sm:p-4 ">{playerBar.node}</div>
                </footer>
            </div>
        </div>
    );
});