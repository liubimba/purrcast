import * as React from "react";
import type {ModuleReportEntity} from "../entity/ModuleReportEntity.ts";
import {ModuleReport} from "./ModuleReport.tsx";

interface InfoProps {
    reports: ModuleReportEntity[];
}


export const Info: React.FC<InfoProps> = ({reports}: InfoProps) => {
    return (
        <div>
            {reports.map(report =>
                <ModuleReport report={report} key={report.module_name}/>
            )}
        </div>
    )
}