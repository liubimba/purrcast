import {StrictMode} from 'react'
import {createRoot} from 'react-dom/client'
import './index.css'
import './assets/noise.png'
import './assets/styles/fonts.css'
import './assets/fonts/syne/Syne-Bold.ttf'
import './assets/fonts/syne/Syne-Regular.ttf'
import './assets/fonts/syne/Syne-Medium.ttf'
import './assets/styles/button.css'
import './assets/styles/cursor.css'
import './assets/styles/card.scss'
import './assets/styles/monitor.css'
import './assets/styles/shadow.css'
import './assets/styles/appLayout.css'


import App from './App.tsx'
import {Provider} from "react-redux";
import {store} from "./store/store.ts";

createRoot(document.getElementById('root')!).render(
    <StrictMode>
        <Provider store={store}>
            {/*<BackgroundLayer/>*/}
            <App/>
        </Provider>
    </StrictMode>,
)
