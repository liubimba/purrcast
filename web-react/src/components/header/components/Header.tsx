import '../styles/header.css'
import {Wave} from "./Wave.tsx";
import {FitText} from "./FitText.tsx";
import {useEffect, useRef, useState} from "react";

interface HeaderProps {
    page: string,
    pages: string[],
    onNavigate: (page: string) => void,
}

interface Rect {
    left: number;
    width: number;
}

export const Header: React.FC<HeaderProps> = ({page, pages, onNavigate}) => {
    const [hoveredPage, setHoveredPage] = useState<string>(page);
    const itemRefs = useRef<Map<string, HTMLButtonElement>>(new Map());
    const [pageRect, setPageRect] = useState<Rect>({left: 0, width: 0});
    const [hoveredRect, setHoveredRect] = useState<Rect>({left: 0, width: 0});
    const fontSize: string = "60px";

    const calculateRect = (page: string) => {
        const el: HTMLButtonElement | undefined = itemRefs.current.get(page);
        if (!el) {
            return {
                left: 0,
                width: 0,
            }
        }
        const parent: HTMLElement = el.parentElement!;
        const parentRect: DOMRect = parent.getBoundingClientRect();
        const rect: DOMRect = el.getBoundingClientRect();
        return {
            left: rect.left - parentRect.left,
            width: rect.width,
        };
    }

    useEffect(() => {

    }, []);

    useEffect(() => {
        setHoveredRect(calculateRect(hoveredPage));
    }, [hoveredPage]);

    useEffect(() => {
        setPageRect(calculateRect(page));
    }, [page]);

    return (
        <div className="header-container">
            <div className="header-block"><Wave/></div>
            <div className="header-main app-border">
                <div
                    className="header-fit-text"
                    style={{
                        left: pageRect.left,
                        width: pageRect.width
                    }}
                >
                    <FitText color={"#4F1A1A"} text={page} fontSize={fontSize}/>
                </div>

                <div
                    className="header-fit-text"
                    style={{
                        left: hoveredRect.left,
                        width: hoveredRect.width
                    }}
                >
                    <FitText color={"#4F1A1A44"} text={hoveredPage} fontSize={fontSize}/>
                </div>

                {pages.map(item => (
                    <button
                        ref={(el) => {
                            if (el) itemRefs.current.set(item, el);
                        }}
                        key={item}
                        className={`header-title ${page === item ? "header-title-active" : ""}`}
                        onClick={() => onNavigate(item)}
                        onMouseEnter={() => setHoveredPage(item)}
                        onMouseLeave={() => setHoveredPage(page)}
                    >
                        {item}
                    </button>
                ))}
            </div>
            <div className="header-block"><Wave/></div>
        </div>
    );
};