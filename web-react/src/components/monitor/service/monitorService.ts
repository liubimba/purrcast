import {Client} from "../../../shared/client/service/client.ts";
import type {MonitorEvents} from "../events/monitorEvents.ts";
import {Logger} from "../../../shared/logger/logger.ts";
import {LoggerFactory} from "../../../shared/logger/loggerFactory.ts";
import {MonitorMessageType} from "../message/monitorMessage.ts";
import type {ModuleReport} from "../module/moduleReport.ts";
import type {Message} from "../../../shared/client/message/message.ts";

export class MonitorService extends Client<MonitorEvents> {
    _logger: Logger;

    public constructor() {
        const logger: Logger = LoggerFactory.getLogger("MonitorService");

        super(logger);

        this._logger = logger;
        this.on("onMessage", (message: Message) => {
            if (message.type === MonitorMessageType.REPORT) {
                try {
                    const rawReports = typeof message.payload === 'string'
                        ? JSON.parse(message.payload)
                        : message.payload;

                    const reports: ModuleReport[] = Array.isArray(rawReports)
                        ? rawReports.map(report => {
                            let moduleStatus;

                            try {
                                moduleStatus = typeof report.module_status === 'string'
                                    ? JSON.parse(report.module_status)
                                    : report.module_status;
                            } catch (e) {
                                this._logger.error("Failed to parse module_status", e);
                                moduleStatus = {state: "unknown", message: "", timestamp: 0};
                            }

                            return {
                                module_name: report.module_name,
                                health_checker: report.health_checker,
                                module_status: {
                                    state: moduleStatus.state || "unknown",
                                    message: moduleStatus.message || "",
                                    timestamp: moduleStatus.timestamp || 0
                                }
                            };
                        })
                        : [];

                    this.emit("onReport", reports);
                } catch (error) {
                    this._logger.error("Failed to parse module reports", error);
                }
            }
        })
    }
}