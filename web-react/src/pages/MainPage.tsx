import {useEffect, useState} from "react";
import {useNavigate} from "react-router-dom";
import {Sidebar} from "../components/Sidebar.tsx";
import {MasterPlayerBar} from "../components/MasterPlayerBar.tsx";
import {Main} from "./Main.tsx";
import {Settings} from "./Settings.tsx";
import {useLogger} from "../hooks/useLogger.ts";
import {selectUserStarted} from "../store/selectors/userSelector.ts";
import {useSelector} from "react-redux";


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
        <div className="content w-full h-full">
            <Sidebar page={page} onNavigate={setPage}/>

            <main className="border-2 " style={{gridArea: "content"}}>
                {page === "main" && <Main/>}
                {page === "settings" && <Settings/>}
            </main>
            <MasterPlayerBar/>
        </div>


    )
}
