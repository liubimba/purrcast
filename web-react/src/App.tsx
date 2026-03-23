import './App.css'
import {BrowserRouter, Route, Routes} from "react-router-dom";
import {WelcomePage} from "./pages/welcome/components/WelcomePage.tsx";
import {MainPage} from "./pages/main/components/MainPage.tsx";

function App() {
    return (
        <BrowserRouter>
            <Routes>
                <Route path="/" element={<WelcomePage/>}/>
                <Route path="/app" element={<MainPage/>}/>
            </Routes>
        </BrowserRouter>
    )
}

export default App
