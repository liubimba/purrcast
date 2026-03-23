import * as React from "react";
import {ModuleReport} from "./ModuleReport.tsx";
import {useMonitor} from "../../monitor/hooks/useMonitor.ts";

export const Info: React.FC<{}> = () => {
    const {reports} = useMonitor();

    return (
        <div className="p-4 flex flex-col  gap-8">
            {reports.map(report =>
                <ModuleReport report={report} key={report.module_name}/>
            )}
        </div>
    )
}