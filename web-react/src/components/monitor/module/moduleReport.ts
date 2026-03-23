import type {ModuleStatus} from "./moduleStatus.ts";

export interface ModuleReport {
    module_name: string;
    health_checker: string;
    module_status: ModuleStatus;
}
