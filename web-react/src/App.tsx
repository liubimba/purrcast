import './App.css'
import {BrowserRouter, Route, Routes} from "react-router-dom";
import {WelcomePage} from "./pages/WelcomePage.tsx";
import {MainPage} from "./pages/MainPage.tsx";
import {BackgroundLayer} from "./components/BackgroundLayer.tsx";

function App() {
    return (
        <BrowserRouter>
            <BackgroundLayer/>
            <Routes>
                <Route path="/" element={<WelcomePage/>}/>
                <Route path="/app" element={<MainPage/>}/>
            </Routes>
        </BrowserRouter>
    )
}

export default App
