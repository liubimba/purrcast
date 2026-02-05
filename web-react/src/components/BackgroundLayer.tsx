import {useEffect} from "react"
import {App} from '../assets/js/background';


export const BackgroundLayer = () => {
    useEffect(() => {
        const app = new App();
        app.init();
    }, []);
    return (
        <div>
            <div className="custom-cursor" id="customCursor"></div>
        </div>
    )
}
