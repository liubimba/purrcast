import * as React from "react";
import type {ModuleReport} from "../module/moduleReport.ts";


export const MonitorCard: React.FC<ModuleReport> = (report: ModuleReport) => {
    const date = new Date(report.module_status.timestamp);

    let color: string = "red";
    if (report.health_checker === "healthy") {
        color = "green";
    } else if (report.module_status.state === "loading") {
        color = "orange";
    }
    return (
        <div className={`card glass p-12 ${color}`}>
            <div className="flex gap-12 justify-between">
                <p>{date.toLocaleDateString('en-EN', {
                    weekday: 'short',
                    year: 'numeric',
                    month: 'long',
                    day: 'numeric',
                    hour: '2-digit',
                    minute: '2-digit'
                })}</p>
                <p className="card-status"> {report.health_checker}</p>
            </div>
            <div className="font-bold text-center text-wrap ">
                <h1 className="card-heading uppercase">{report.module_name}</h1>
                <h2 className="capitalize">{report.module_status.state}</h2>
                <p className="font-light">{report.module_status.message}</p>
            </div>
        </div>
    )
}