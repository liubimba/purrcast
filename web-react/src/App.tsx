import './App.css'
import {BrowserRouter, Route, Routes} from "react-router-dom";
import {WelcomePage} from "./pages/WelcomePage.tsx";
import {MainPage} from "./pages/MainPage.tsx";
import {StartedProvider} from "./components/StartedProvider.tsx";
import {useEffect} from "react";
import type {AppDispatch} from "./store/store.ts";
import {useDispatch} from "react-redux";
import {configurationSlice} from "./store/configurationSlice.ts";

function App() {
    const dispatch: AppDispatch = useDispatch();
    useEffect(() => {
        dispatch(configurationSlice.actions.connect());
    }, [dispatch])
    return (
        <BrowserRouter>
            <StartedProvider>
                <Routes>
                    <Route path="/" element={<WelcomePage/>}/>
                    <Route path="/app" element={<MainPage/>}/>
                </Routes>
            </StartedProvider>
        </BrowserRouter>
    )
}

export default App
