import {useContext, useEffect, useState} from "react";
import {useNavigate} from "react-router-dom";
import {StartedContext} from "../context/StartedContext.tsx";
import {Sidebar} from "../components/Sidebar.tsx";
import {MasterPlayerBar} from "../components/MasterPlayerBar.tsx";
import {Main} from "./Main.tsx";
import {Settings} from "./Settings.tsx";
import {useLogger} from "../hooks/useLogger.ts";


export const MainPage: React.FC = () => {
    const logger = useLogger("MainPage");

    const {started} = useContext(StartedContext);
    const navigate = useNavigate();
    const [page, setPage] = useState("main");

    useEffect(() => {
        if (!started) {
            logger.warn("Redirect to welcome page");
            navigate("/");
        }
    }, [started, navigate]);


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
