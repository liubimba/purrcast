const fs = require('fs');
const fse = require('fs-extra');
const path = require('path');

const rootPath = path.join(__dirname, "..", "..");
const artifactsPath = path.join(rootPath, "artifacts");
const resourcesPath = path.join(__dirname, "..", "resources");

async function main() {
    console.log("Copying artifacts into Electron resources...");

    await fse.remove(resourcesPath);
    await fse.ensureDir(resourcesPath);

    const iconSourceDir = path.join(__dirname, "..", "src", "assets", "icons");
    const iconTargetDir = path.join(resourcesPath, "icons");
    await fse.ensureDir(iconTargetDir);
    const iconFiles = fs.readdirSync(iconSourceDir);
    for (const file of iconFiles) {
        const src = path.join(iconSourceDir, file);
        const dest = path.join(iconTargetDir, file);
        console.log(`Copying ${src} to ${dest}`);
        await fse.copy(src, dest);
    }

    const binTarget = path.join(resourcesPath, "bin");
    await fse.ensureDir(binTarget);

    const launcherBinPath = path.join(artifactsPath, "launcher-cpp", "bin");
    const launcherFiles = fs.readdirSync(launcherBinPath);

    for (const file of launcherFiles) {
        const src = path.join(launcherBinPath, file);
        const dest = path.join(binTarget, file);
        await fse.copy(src, dest);
    }

    const backendSrc = path.join(artifactsPath, "backend-go", "backend-go");
    const backendDest = path.join(binTarget, "backend-go");
    await fse.copy(backendSrc, backendDest);

    await fse.copy(
        path.join(artifactsPath, "web-react"),
        path.join(resourcesPath, "static")
    );

    await fse.copy(
        path.join(artifactsPath, "config"),
        path.join(resourcesPath, "config"),
    );

    console.log("Artifacts copied into Electron resources");
}

main();