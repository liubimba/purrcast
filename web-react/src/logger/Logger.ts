export class Logger {
    private _name: string;

    constructor(name: string) {
        this._name = name;
    }

    public info(...data: any[]): void {
        console.info(this.localized("INFO"), ...data);
    }

    public error(...data: any[]): void {
        console.error(this.localized("ERROR"), ...data);
    }

    public warn(...data: any[]): void {
        console.warn(this.localized("WARN"), ...data);
    }

    public debug(...data: any[]): void {
        console.debug(this.localized("DEBUG"), ...data);
    }


    public get name(): string {
        return this._name;
    }

    private localized(level: string): string {
        return `[${this._name}][${level}]:`;
    }
}