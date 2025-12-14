import * as React from "react";
import {useContext} from "react";
import {useNavigate} from "react-router-dom";
import {StartedContext} from "../context/StartedContext.tsx";

interface WelcomePageProps {
}

export const WelcomePage: React.FC<WelcomePageProps> = ({}) => {
    const navigate = useNavigate();
    const {start} = useContext(StartedContext);

    const onClick = () => {
        start();

        navigate("/app");
    }

    return (
        <div>
            <button onClick={onClick}>Connect</button>
        </div>
    )
}