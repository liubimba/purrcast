import {useEffect} from "react";
import {useNavigate} from "react-router-dom";
import {MasterPlayerBar} from "../components/MasterPlayerBar.tsx";
import {useLogger} from "../hooks/useLogger.ts";
import {selectUserStarted} from "../store/selectors/userSelector.ts";
import {useSelector} from "react-redux";
import {AppLayout} from "../app/AppLayout.tsx";
import {Main} from "./Main.tsx";
import {Info} from "../info/components/Info.tsx";
import {Header} from "../header/components/Header.tsx";
import {About} from "../about/components/About.tsx";


const paginator = {
    page: "main",
    pages: ["multiroom", "about"],
    setPage: (page: string) => {

    }
}

export const MainPage: React.FC = () => {
    const logger = useLogger("MainPage");
    const clientStarted = useSelector(selectUserStarted());
    const navigate = useNavigate();

    useEffect(() => {
        if (!clientStarted) {
            logger.warn("Redirect to welcome page");
            navigate("/");
        }
    }, [clientStarted, navigate]);


    return (
        <AppLayout
            info={<Info/>}
            header={<Header page={paginator.page} pages={paginator.pages} onNavigate={paginator.setPage}/>}
            playerbar={<MasterPlayerBar/>}
            content={paginator.page == "main" ? <Main/> : paginator.page === "about" ? <About/> : <></>}
        >
        </AppLayout>
    )
}
