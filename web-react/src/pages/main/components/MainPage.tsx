import {useEffect, useRef, useState} from "react";
import {useLogger} from "../../../hooks/useLogger.ts";
import {selectUserStarted} from "../../../store/selectors/userSelector.ts";
import {useSelector} from "react-redux";
import {AppLayout} from "./AppLayout.tsx";
import {Info} from "../../../components/info/components/Info.tsx";
import {Header} from "../../../components/header/components/Header.tsx";
import {About} from "../../about/components/About.tsx";
import {Controls} from "../../../components/controls/components/Controls.tsx";
import {MasterPlayer} from "../../../components/master/components/MasterPlayer.tsx";
import {ConnectionsBoard} from "../../../components/connections/components/ConnectionsBoard.tsx";
import {useNavigateWithQuery} from "../../../hooks/useNavigate.ts";


class Paginator {
    private _page: string;
    private readonly _pages: string[];

    constructor(page: string, pages: string[]) {
        this._page = page;
        this._pages = pages;
        this.setPage = this.setPage.bind(this);
    }

    public setPage(page: string): void {
        if (!this._pages.includes(page)) {
            throw new Error("page doesn't exist");
        }
        this._page = page;
    }

    public get page(): string {
        return this._page;
    }

    public get pages(): string[] {
        return this._pages;
    }
}


export const MainPage: React.FC = () => {
    const logger = useLogger("MainPage");
    const clientStarted = useSelector(selectUserStarted());
    const navigate = useNavigateWithQuery();


    const [page, setPage] = useState("purrcast");
    const paginator = useRef<Paginator>(new Paginator("purrcast", ["purrcast", "about"]));

    const handleNavigate = (p: string): void => {
        paginator.current.setPage(p);
        setPage(p);  // триггерим ре-рендер
    };

    useEffect(() => {
        if (!clientStarted) {
            logger.warn("Redirect to welcome page");
            navigate("/");
        }
    }, [page]);

    return (
        <AppLayout
            info={{
                node: <Info/>,
                title: "MONITOR"
            }}

            header={<Header page={page} pages={paginator.current.pages} onNavigate={handleNavigate}/>}

            playerBar={{
                node: <MasterPlayer/>,
                title: "MASTER"
            }}

            content={
                page == "purrcast" ? {
                    node: <Controls/>,
                    title: "CONTROLS"
                } : page === "about" ? {
                    node: <About/>,
                    title: "ABOUT"
                } : {
                    node: <></>,
                    title: "UNDEFINED"
                }}

            connectionsBoard={{
                node: <ConnectionsBoard/>,
                title: "CONNECTIONS"
            }}
        >
        </AppLayout>
    )
}
