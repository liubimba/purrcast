const fs = require('fs');
const fse = require('fs-extra');
const path = require('path');

const root = path.join(__dirname, "..", "..");
const artifactsPath = path.join(root, "artifacts");
const targetPath = path.join(__dirname, "..", "resources");

async function main() {
    console.log("Copying artifacts into Electron resources...");

    await fse.remove(targetPath);
    await fse.ensureDir(targetPath);

    const binTarget = path.join(targetPath, "bin");
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
        path.join(targetPath, "static")
    );

    await fse.copy(
        path.join(artifactsPath, "config"),
        path.join(targetPath, "config"),
    );

    console.log("Artifacts copied into Electron resources");
}

main();