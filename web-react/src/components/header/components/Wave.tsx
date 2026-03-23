import * as React from "react";
import "../styles/wave.css"

export const Wave: React.FC = () => {
    return (
        <div
            className="app-border wave"
            style={{}}
        >
            <svg
                viewBox="0 0 1200 120"
                preserveAspectRatio="none"
                xmlns="http://www.w3.org/2000/svg"
                style={{
                    position: "absolute",
                    bottom: 0,
                    left: 0,
                    width: "200%", // двойная ширина для бесшовной анимации
                    height: "100%",
                    animation: "waveMove 10s linear infinite",
                }}
            >
                <path
                    d="
                        M0,90
                        C60,85 80,70 120,40
                        C160,10 180,8 200,35
                        C220,60 240,75 300,80
                        C360,85 400,82 450,55
                        C490,30 510,15 540,20
                        C570,25 590,50 640,70
                        C680,85 720,90 760,75
                        C800,58 820,30 860,18
                        C900,8 920,10 950,30
                        C980,50 1000,72 1060,82
                        C1100,88 1140,85 1200,90

                        C1260,85 1280,70 1320,40
                        C1360,10 1380,8 1400,35
                        C1420,60 1440,75 1500,80
                        C1560,85 1600,82 1650,55
                        C1690,30 1710,15 1740,20
                        C1770,25 1790,50 1840,70
                        C1880,85 1920,90 1960,75
                        C2000,58 2020,30 2060,18
                        C2100,8 2120,10 2150,30
                        C2180,50 2200,72 2260,82
                        C2300,88 2340,85 2400,90

                        L2400,120 L0,120 Z
                    "
                    fill="#f5f0e8"
                />
            </svg>

            <style>{`
        @keyframes waveMove {
          0%   { transform: translateX(0); }
          100% { transform: translateX(-50%); }
        }
      `}</style>
        </div>
    );
};