import "../styles/fit.css"
import {useEffect, useRef, useState} from "react";

export const FitText: React.FC<{ text: string, fontSize: string, color?: string }> = ({
                                                                                          text,
                                                                                          fontSize,
                                                                                          color = "black"
                                                                                      }) => {
    const containerRef = useRef<HTMLDivElement>(null);
    const [opacity, setOpacity] = useState<number>(1);
    const timerRef = useRef<ReturnType<typeof setTimeout>>(0);

    const split = (str: string) => {
        const middle: number = Math.floor(str.length / 2);

        const part1: string = str.slice(0, middle);
        const part2: string = str.slice(middle);

        return [part1, part2];
    }
    useEffect(() => {
        clearTimeout(timerRef.current);
        setOpacity(0);
        timerRef.current = setTimeout(() => {
            setOpacity(1);
        }, 200);
        return () => clearTimeout(timerRef.current);
    }, [text]);

    return (
        <div
            ref={containerRef}
            className="fit-text-container"
            style={{opacity}}>
            {split(text.toUpperCase()).map((chunk, i) => (
                <div
                    className="fit-text"
                    key={i}
                    style={{
                        color: color,
                        fontSize: `${fontSize}`
                    }}
                >
                    {chunk}
                </div>
            ))}
        </div>
    );
};