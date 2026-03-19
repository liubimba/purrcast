import * as React from "react";

export const WavyBorder: React.FC<{ animate?: boolean, clockwise?: boolean, gap?: number, length?: number }> = ({
                                                                                                                    animate = false,
                                                                                                                    clockwise = false,
                                                                                                                    gap = 50,
                                                                                                                    length = 100
                                                                                                                }) => {
    const id: string = `dashmove-${Math.round(Math.random() * 10000)}`;
    const offset: number = Math.round(Math.random() * 10000);

    return (
        <div className="w-full h-full">
            <svg
                width="100%"
                height="100%"
                xmlns="http://www.w3.org/2000/svg">
                <defs>
                    <filter id="rough">
                        <feTurbulence type="turbulence" baseFrequency="0.03" numOctaves="2" result="turb"/>
                        <feDisplacementMap in2="turb" in="SourceGraphic" scale="2" xChannelSelector="R"
                                           yChannelSelector="G"/>
                    </filter>
                </defs>
                <filter id="displacementFilter">
                    <feTurbulence
                        type="turbulence"
                        baseFrequency="0.05"
                        numOctaves="1"
                        result="turbulence"/>
                    <feDisplacementMap
                        in2="turbulence"
                        in="SourceGraphic"
                        scale="1.4"
                        xChannelSelector="B"
                        yChannelSelector="G"/>
                </filter>
                <rect
                    filter="url(#rough)"
                    x="0"
                    y="0"
                    width="100%"
                    height="100%"
                    rx="0"
                    ry="0"
                    fill="none"
                    stroke="var(--color-secondary)"
                    strokeWidth="4"
                    strokeDasharray={`${length},${gap}`}
                    strokeLinecap="round"
                    style={animate ? {
                        animation: `${id} 2s steps(35)  infinite`,
                    } : {
                        strokeDashoffset: offset,
                    }}
                />
                <style>
                    {`
                          @keyframes ${id} {
                          from{
                            stroke-dashoffset: 0;
                          }
                            to {
                              stroke-dashoffset: ${(gap + length) * (clockwise ? -1 : 1)};
                            }
                          }
                        `}
                </style>

            </svg>
        </div>
    )
}