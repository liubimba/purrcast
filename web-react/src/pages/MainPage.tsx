import {useEffect, useRef, useState} from "react";
import {useNavigate} from "react-router-dom";
import {useLogger} from "../hooks/useLogger.ts";
import {selectUserStarted} from "../store/selectors/userSelector.ts";
import {useSelector} from "react-redux";
import {AppLayout} from "../app/AppLayout.tsx";
import {Info} from "../info/components/Info.tsx";
import {Header} from "../header/components/Header.tsx";
import {About} from "../about/components/About.tsx";
import {Controls} from "../controls/components/Controls.tsx";
import {MasterPlayer} from "../master/components/MasterPlayer.tsx";
import {useMasterPlayer} from "../hooks/useMasterPlayer.ts";
import type {ModuleReportEntity} from "../info/entity/ModuleReportEntity.ts";


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
    const navigate = useNavigate();


    const [page, setPage] = useState("multiroom");
    const paginator = useRef<Paginator>(new Paginator("multiroom", ["multiroom", "about"]));

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

    const {masterVolume, masterMuted, updateMasterVolume, updateMasterMuted} = useMasterPlayer();

    const TEST_MODULE_REPORT: ModuleReportEntity = {
        module_name: "Module name",
        health_checker: "Health checker",
        module_status: {
            state: "failed",
            timestamp: Date.now().toString(),
            message: "Lorem ipsum dolor sit amet, consetetur",
        }
    }


    return (
        <AppLayout
            info={<Info reports={[TEST_MODULE_REPORT]}/>}
            header={<Header page={page} pages={paginator.current.pages} onNavigate={handleNavigate}/>}
            playerbar={<MasterPlayer gain={10} volume={masterVolume} muted={masterMuted}
                                     onVolumeChange={updateMasterVolume}
                                     onMutedChange={updateMasterMuted}/>}
            content={page == "multiroom" ? <Controls/> : page === "about" ? <About/> : <></>}
        >
        </AppLayout>
    )
}
