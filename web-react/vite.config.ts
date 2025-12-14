import {defineConfig} from 'vite'
import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite'
import vitePluginSvgr from "vite-plugin-svgr";
// https://vite.dev/config/
export default defineConfig({
    build: {
        outDir: process.env.OUT_DIR || './dist',
    },
    server: {
        host: '127.0.0.1',
    },
    plugins: [react(), tailwindcss(), vitePluginSvgr()]
})
