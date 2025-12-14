import {Logger} from "./Logger.ts";

export class LoggerFactory {
    private static _instance: LoggerFactory = new LoggerFactory();
    private _loggers: Logger[];

    public static getLogger(name: string): Logger {
        return LoggerFactory._instance._getLogger(name);
    }

    private constructor() {
        this._loggers = [];
    }

    private _getLogger(name: string): Logger {
        for (const logger of this._loggers) {
            if (logger.name === name) {
                return logger;
            }
        }
        const logger: Logger = new Logger(name);
        this._loggers.push(logger);
        return logger;
    }
}