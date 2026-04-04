import * as path from "node:path";
import {app} from "electron";
import {spawn} from "node:child_process";
import log from "electron-log/main";
import {ChildProcessWithoutNullStreams} from "child_process";
import {EventEmitter} from "node:events";
import * as net from "node:net";

type ServerProps = {
    path: string,
    port: number,
    static: string
}

type SnapserverProps = {
    path: string,
    config: string
}

type SnapclientProps = {
    path: string,
}

type LauncherProps = {
    path: string,
}

type MonitorProps = {
    port: number
}

type Props = {
    server: ServerProps;
    launcher: LauncherProps;
    snapserver: SnapserverProps;
    snapclient: SnapclientProps;
    monitor: MonitorProps;
}

class Port {
    public static isBound(
        port: number,
        host: string,
        timeout: number
    ): Promise<boolean> {
        return new Promise((resolve) => {
            const socket = new net.Socket();

            const done = (result: boolean) => {
                socket.destroy();
                resolve(result);
            };

            socket.setTimeout(timeout);

            socket.once('connect', () => {
                done(true);
            });

            socket.once('timeout', () => {
                done(false);
            });

            socket.once('error', () => {
                done(false);
            });
            socket.connect(port, host);
        });
    }
}

class PortManager {
    private readonly _range: number[];
    private readonly _allocated: number[];
    private readonly _host: string;
    private readonly _timeout: number;

    private constructor(range: number[]) {
        this._range = range;
        this._allocated = [];
        this._host = "127.0.0.1";
        this._timeout = 1000;
    }

    public static fromTo(from: number, to: number): PortManager {
        return new PortManager(Array.from({length: Math.floor((to - from)) + 1}, (_, i) => from + i))
    }

    public async allocate(): Promise<number> {
        for (let index = 0; index < this._range.length; index++) {
            if (this._allocated.includes(this._range[index])) continue;
            if (!(await this.isBound(this._range[index]))) {
                this._allocated.push(this._range[index]);
                return this._range[index];
            }
        }
        throw new Error("There are no free ports");
    }

    public async isBound(port: number): Promise<boolean> {
        return Port.isBound(port, this._host, this._timeout);
    }
}

interface LauncherEvents {
    ready: (port: number) => void;
}

export interface Launcher {
    on<U extends keyof LauncherEvents>(
        event: U,
        listener: LauncherEvents[U]
    ): this;

    emit<U extends keyof LauncherEvents>(
        event: U,
        ...args: Parameters<LauncherEvents[U]>
    ): boolean;
}


export class Launcher extends EventEmitter implements Launcher {
    private _isDev: boolean;
    private _process: ChildProcessWithoutNullStreams | null;
    private _portManager: PortManager;

    constructor(isDev: boolean) {
        super();
        this._isDev = isDev;
        this._portManager = PortManager.fromTo(5000, 9000);
    }

    public async start() {
        if (this._isDev) {
            await this.startDev_();
        } else {
            await this.startProd_();
        }
    }

    public async stop() {
        if (!this._process) {
            return;
        }
        return new Promise((resolve) => {
            this._process.once('exit', () => {
                resolve(true);
            });
            const timeout = setTimeout(() => {
                log.warn("Process did not exit gracefully, forcing kill");
                this._process?.kill("SIGKILL");
                resolve(true);
            }, 5000);
            this._process.once('exit', () => {
                clearTimeout(timeout);
                resolve(true);
            });

            this._process.kill('SIGTERM');
        })
    }

    private async startDev_() {
        const artifacts = path.join(app.getAppPath(), "..", "artifacts");

        this.start_({
            server: {
                path: path.join(artifacts, "backend-go", "backend-go"),
                port: await this._portManager.allocate(),
                static: path.join(artifacts, "web-react"),
            },
            snapclient: {
                path: path.join(artifacts, "launcher-cpp", "bin", "snapclient")
            },
            snapserver: {
                path: path.join(artifacts, "launcher-cpp", "bin", "snapserver"),
                config: path.join(artifacts, "config", "snapserver.conf")
            },
            launcher: {
                path: path.join(artifacts, "launcher-cpp", "bin", "multiroom")
            },
            monitor: {
                port: await this._portManager.allocate(),
            }
        })
    }

    private async startProd_() {
        this.start_({
            server: {
                path: path.join(process.resourcesPath, "bin", "backend-go"),
                port: await this._portManager.allocate(),
                static: path.join(process.resourcesPath, "static")
            },
            launcher: {
                path: path.join(process.resourcesPath, "bin", "multiroom")
            },
            snapclient: {
                path: path.join(process.resourcesPath, "bin", "snapclient")
            },
            snapserver: {
                path: path.join(process.resourcesPath, "bin", "snapserver"),
                config: path.join(process.resourcesPath, "config", "snapserver.conf")
            },
            monitor: {
                port: await this._portManager.allocate(),
            }
        })
    }

    private start_(props: Props) {
        const launcherProps: LauncherProps = props.launcher;
        const snapserverProps: SnapserverProps = props.snapserver;
        const snapclientProps: SnapclientProps = props.snapclient;
        const serverProps: ServerProps = props.server;
        const monitorProps: MonitorProps = props.monitor;
        this._process = spawn(launcherProps.path, [
            "--server.path", serverProps.path,
            "--server.port", serverProps.port.toString(),
            "--server.static_dir", serverProps.static,
            "--snapserver.path", snapserverProps.path,
            "--snapserver.config", snapserverProps.config,
            "--snapclient.path", snapclientProps.path,
            "--monitor.port", monitorProps.port.toString(),
        ], {
            cwd: path.dirname(launcherProps.path),
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

        this.inspect_(serverProps.port);
    }

    private inspect_(port: number) {
        setTimeout(async () => {
            if (await this._portManager.isBound(port)) {
                this.emit("ready", port);
            } else {
                this.inspect_(port);
            }
        }, 1000);
    }
}