import {useEffect, useState} from "react";
import {useNavigate} from "react-router-dom";
import {Sidebar} from "../components/Sidebar.tsx";
import {MasterPlayerBar} from "../components/MasterPlayerBar.tsx";
import {useLogger} from "../hooks/useLogger.ts";
import {selectUserStarted} from "../store/selectors/userSelector.ts";
import {useSelector} from "react-redux";
import {AppLayout} from "../app/AppLayout.tsx";
import {Monitor} from "./Monitor.tsx";
import {Main} from "./Main.tsx";
import {Settings} from "./Settings.tsx";


export const MainPage: React.FC = () => {
    const logger = useLogger("MainPage");
    const clientStarted = useSelector(selectUserStarted());
    const navigate = useNavigate();
    const [page, setPage] = useState("main");

    useEffect(() => {
        if (!clientStarted) {
            logger.warn("Redirect to welcome page");
            navigate("/");
        }
    }, [clientStarted, navigate]);


    return (
        <AppLayout
            sidebar={<Sidebar page={page} onNavigate={setPage}/>}
            playerbar={<MasterPlayerBar/>}
            content={page == "main" ? <Main/> : page === "monitor" ? <Monitor/> : page === "settings" ? <Settings/> :
                <div/>}
        >
        </AppLayout>
    )
}
