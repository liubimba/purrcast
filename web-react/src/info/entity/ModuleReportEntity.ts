import type {ModuleStatusEntity} from "./ModuleStatusEntity.ts";

export interface ModuleReportEntity {
    module_name: string;
    health_checker: string;
    module_status: ModuleStatusEntity;
}
