import type {ModuleReport} from "../module/moduleReport.ts";
import type {ClientEvents} from "../../../shared/client/events/clientEvents.ts";

export interface MonitorEvents extends ClientEvents {
    onReport: (reports: ModuleReport[]) => void;
}
