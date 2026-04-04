/**
 * This file will automatically be loaded by webpack and run in the "renderer" context.
 * To learn more about the differences between the "main" and the "renderer" context in
 * Electron, visit:
 *
 * https://electronjs.org/docs/latest/tutorial/process-model
 *
 * By default, Node.js integration in this file is disabled. When enabling Node.js integration
 * in a renderer process, please be aware of potential security implications. You can read
 * more about security risks here:
 *
 * https://electronjs.org/docs/tutorial/security
 *
 * To enable Node.js integration in this file, open up `main.js` and enable the `nodeIntegration`
 * flag:
 *
 * ```
 *  // Create the browser window.
 *  mainWindow = new BrowserWindow({
 *    width: 800,
 *    height: 600,
 *    webPreferences: {
 *      nodeIntegration: true
 *    }
 *  });
 * ```
 */

import './index.css';
import textGif from './assets/gif/text_preload.gif'
import catGif from './assets/gif/cat_preload.gif'

console.log(
    '👋 This message is being logged by "renderer.js", included via webpack',
);

const textPreloadImg = document.getElementById("textPreload") as HTMLImageElement
if (textPreloadImg) {
    textPreloadImg.src = textGif
} else {
    console.error("Failed to find #textPreload image element");
}

const catPreloadImg = document.getElementById("catPreload") as HTMLImageElement
if (catPreloadImg) {
    catPreloadImg.src = catGif;
} else {
    console.error("Failed to find #catPreload image element");
}