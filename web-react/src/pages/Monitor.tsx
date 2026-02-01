import * as React from "react";
import {useMonitor} from "../monitor/hooks/useMonitor.ts";
import {MonitorCard} from "../monitor/components/MonitorCard.tsx";

interface MonitorProps {
}

export const Monitor: React.FC<MonitorProps> = ({}) => {
    const {reports} = useMonitor();

    return (
        <div className="flex flex-wrap w-full h-full justify-center gap-16 p-16">
            <div
                className="wrap-break-word monitor-heading flex items-center font-bold w-full h-full top-0 absolute text-center">
                <div className="w-full">
                    <h1>MODULES</h1>
                </div>
            </div>
            {reports.map((report) => (
                <MonitorCard key={report.module_name} module_name={report.module_name}
                             health_checker={report.health_checker} module_status={report.module_status}/>

            ))}
        </div>

    )
}