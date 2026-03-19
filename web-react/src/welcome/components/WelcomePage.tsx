import * as React from "react";
import {useNavigate} from "react-router-dom";
import {useClient} from "../../hooks/useClient.ts";
import {WavyBorder} from "./WavyBorder.tsx";
import {APP_NAME} from "../../constants/storage.ts";

import cat1 from "../assets/images/cat1.png";
import cat2 from "../assets/images/cat2.png";
import cat3 from "../assets/images/cat3.png";

const Border: React.FC<{
    className?: string;
    clockwise?: boolean;
    animate?: boolean,
    gap?: number,
    length?: number
}> = ({
          className = "",
          clockwise = false,
          animate = true,
          length = 100,
          gap = 50
      }) => {
    return (
        <div
            className={`absolute inset-0  pointer-events-none flex items-center justify-center w-full h-full ${className}`}>
            <WavyBorder gap={gap} length={length} animate={animate} clockwise={clockwise}/>
        </div>
    );
};

export const WelcomePage: React.FC<{}> = () => {
    const navigate = useNavigate();
    const {setClientStarted} = useClient();

    const onClick = () => {
        setClientStarted(true);
        navigate("/app");
    }

    const buttonText: string = "JOIN";
    return (
        <div className="bg-primary w-full h-full flex justify-center items-center relative">
            <Border className="scale-90" animate={false} length={400} gap={300}/>
            <Border className="scale-95" animate={false} length={400} gap={300}/>
            <Border className="scale-85" animate={false} length={400} gap={300}/>

            <div className="absolute flex w-full h-full justify-between gap-40">
                <div className={`flex-1   h-1/2 flex items-end justify-end`}>
                    <img className="max-w-[400px] object-contain" src={cat2} width="100%" height="100%"/>
                </div>
                <div className={`flex-1   h-1/2 flex items-start justify-center`}>
                    <img className="max-w-[300px] object-contain" src={cat1} width="100%" height="100%"/>
                </div>
                <div className={`flex-1   h-1/2 flex items-end justify-start`}>
                    <img className="max-w-[300px] object-contain" src={cat3} width="100%" height="100%"/>
                </div>
            </div>

            <div className="relative">
                <button onClick={onClick} className="group ">
                    <div
                        className="z-50 transition-all duration-75 relative w-full h-full bg-primary p-16 group-hover:[text-shadow:-12px_12px_0_var(--color-secondary)]">
                        <Border animate={true} clockwise={true}/>
                        <Border
                            className={`transition-transform duration-75 group-hover:-translate-x-[12px] group-hover:translate-y-[12px]`}
                            animate={true} clockwise={true}/>
                        <Border
                            className={`transition-transform duration-75 group-hover:-translate-x-[24px] group-hover:translate-y-[24px]`}
                            animate={true} clockwise={true}/>

                        <div
                            className=" top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2  absolute text-primary scale-[105%]">
                            <h2 className="font-black  [-webkit-text-stroke:6px_var(--color-primary)]  text-transparent text-[6vw]">{buttonText}</h2>
                        </div>
                        <div className="relative">
                            <h2 className="font-black text-[6vw]  ">{buttonText}</h2>
                        </div>
                    </div>
                </button>

            </div>

            <div className="absolute bottom-0 w-full h-1/2 flex justify-center items-center">
                <svg viewBox="0 0 1840 788" fill="none" xmlns="http://www.w3.org/2000/svg"
                     className="w-full h-full absolute inset-0 " preserveAspectRatio="none">
                    <path
                        d="M1839.5 787.5H0L181.5 592.5L64 476L208 413.5L213.5 318H328.5V242L482 303L643 242L917.75 0L1192.5 242L1354.5 303L1512.5 242V318L1629 326.5V418L1764.5 476L1659.5 592.5L1839.5 787.5Z"
                        fill="var(--color-shadow)"/>
                </svg>
                <h1 className="text-[4vw] font-black text-primary text-primary relative">
                    {APP_NAME}
                </h1>
            </div>
        </div>
    )
}

