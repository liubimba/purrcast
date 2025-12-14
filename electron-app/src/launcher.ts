import * as path from "node:path";
import {app} from "electron";
import {spawn} from "node:child_process";
import log from "electron-log/main";
import {ChildProcessWithoutNullStreams} from "child_process";

type BinaryPaths = {
    server: string;
    snapserver: string;
    snapclient: string;
    launcher: string;
}

export class Launcher {
    private _isDev: boolean;
    private _process: ChildProcessWithoutNullStreams | null;

    constructor(isDev: boolean) {
        this._isDev = isDev;
    }

    public async start() {
        if (this._isDev) {
            this.startDev_();
        } else {
            this.startProd_();
        }
    }

    public async stop() {
        this._process?.kill();
    }

    private startDev_() {
        const artifacts = path.join(app.getAppPath(), "..", "artifacts");

        this.start_({
            server: path.join(artifacts, "backend-go", "backend"),
            snapclient: path.join(artifacts, "launcher-cpp", "bin", "snapclient"),
            snapserver: path.join(artifacts, "launcher-cpp", "bin", "snapserver"),
            launcher: path.join(artifacts, "launcher-cpp", "bin", "multiroom"),
        })
    }

    private startProd_() {
        this.start_({
            server: path.join(process.resourcesPath, "bin", "backend"),
            launcher: path.join(process.resourcesPath, "bin", "multiroom"),
            snapclient: path.join(process.resourcesPath, "bin", "snapclient"),
            snapserver: path.join(process.resourcesPath, "bin", "snapserver"),
        })
    }

    private start_(paths: BinaryPaths) {
        this._process = spawn(paths.launcher, [
            "--server.path", paths.server,
            "--snapserver.path", paths.snapserver,
            "--snapclient.path", paths.snapclient,
        ], {
            cwd: path.dirname(paths.launcher),
        })
        this._process.stdout.on('data', (data: Buffer) => {
            console.log(data.toString());
        })
        this._process.stderr.on('data', (data: Buffer) => {
            console.log(data.toString());
        })
        this._process.on('spawn', () => {
            log.info("Spawned process:", this._process.spawnfile);
        })
        this._process.on('close', () => {
            log.info("Closed process:", this._process.spawnfile);
        })
        this._process.on('exit', (code: number) => {
            log.info("Process exit code:", code);
        })
        this._process.on('error', (err: Error) => {
            log.error("Spawned process error:", err);
        })
        this._process.on('disconnect', () => {
            log.info("Spawned process disconnected");
        })
        log.info("Spawn process:", this._process.spawnfile, this._process.spawnargs);
    }
}