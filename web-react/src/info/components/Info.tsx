import * as React from "react";
import type {ModuleReportEntity} from "../entity/ModuleReportEntity.ts";
import {ModuleReport} from "./ModuleReport.tsx";

interface InfoProps {
    reports: ModuleReportEntity[];
}


export const Info: React.FC<InfoProps> = ({reports}: InfoProps) => {
    return (
        <div className="p-4 flex flex-col  gap-8">
            {reports.map(report =>
                <ModuleReport report={report} key={report.module_name}/>
            )}
            {reports.map(report =>
                <ModuleReport report={report} key={report.module_name}/>
            )}
            {reports.map(report =>
                <ModuleReport report={report} key={report.module_name}/>
            )}
            {reports.map(report =>
                <ModuleReport report={report} key={report.module_name}/>
            )}
            {reports.map(report =>
                <ModuleReport report={report} key={report.module_name}/>
            )}
        </div>
    )
}