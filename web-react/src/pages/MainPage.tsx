import {useEffect, useState} from "react";
import {useNavigate} from "react-router-dom";
import {Sidebar} from "../components/Sidebar.tsx";
import {MasterPlayerBar} from "../components/MasterPlayerBar.tsx";
import {Main} from "./Main.tsx";
import {Settings} from "./Settings.tsx";
import {useLogger} from "../hooks/useLogger.ts";
import {selectUserStarted} from "../store/selectors/userSelector.ts";
import {useSelector} from "react-redux";
import {Monitor} from "./Monitor.tsx";
import {AppLayout} from "../app/AppLayout.tsx";


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
        <AppLayout sidebar={<Sidebar page={page} onNavigate={setPage}/>}
                   playerbar={<MasterPlayerBar/>}
                   content={<main
                       className="flex h-full justify-center items-center justify-items-center"
                       style={{gridArea: "content", borderRadius: "16px"}}>
                       {page === "main" && <Main/>}
                       {page === "monitor" && <Monitor/>}
                       {page === "settings" && <Settings/>}

                   </main>}>
        </AppLayout>
        // <div className="main-container gap-4 grid w-full  p-12">
        //     <aside style={{gridArea: "sidebar"}}>
        //         <Glass className="h-full">
        //             <Sidebar page={page} onNavigate={setPage}/>
        //         </Glass>
        //
        //     </aside>
        //
        //     <Glass className="overflow-x-hidden overflow-y-auto">
        //         <main
        //             className="flex h-full justify-center items-center justify-items-center"
        //             style={{gridArea: "content", borderRadius: "16px"}}>
        //             {page === "main" && <Main/>}
        //             {page === "monitor" && <Monitor/>}
        //             {page === "settings" && <Settings/>}
        //
        //         </main>
        //     </Glass>
        //
        //
        //     <section style={{gridArea: "playerbar", position: "relative"}}>
        //         <Glass className="h-full">
        //             <MasterPlayerBar/>
        //         </Glass>
        //     </section>
        //
        // </div>


    )
}
