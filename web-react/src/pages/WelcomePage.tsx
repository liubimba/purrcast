import * as React from "react";
import {useNavigate} from "react-router-dom";
import {useClient} from "../hooks/useClient.ts";


export const WelcomePage: React.FC<{}> = ({}) => {
    const navigate = useNavigate();
    const {setClientStarted} = useClient();

    const onClick = () => {
        setClientStarted(true);

        navigate("/app");
    }

    return (
        <div>
            <button onClick={onClick}>Connect</button>
        </div>
    )
}