import * as React from "react";
import type {ModuleReportEntity} from "../entity/ModuleReportEntity.ts";
import "../styles/moduleReport.css"

export const ModuleReport: React.FC<{ report: ModuleReportEntity }> = ({report}) => {
    const date = new Date(Number.parseInt(report.module_status.timestamp));

    const mapModuleStatusToColor = (status: string) => {
        if (status.toLowerCase() === "running") {
            return "bg-success";
        } else if (status.toLowerCase() === "failed") {
            return "bg-error";
        }
        return "#4B4B4B";
    }

    return (
        <div className="module-report-container">
            <div className="flex module-report-wrapper bg-primary min-w-fit">
                <div className="flex flex-col flex-3 min-w-0">
                    <div className="flex items-center flex-1">
                        <h2 className="text-[clamp(12px,2cqi,40px)] font-black">{report.module_name}</h2>

                    </div>
                    <div className="border-t-1 flex-1 items-end  flex">
                        <p className="font-bold">{date.toLocaleDateString('en-EN', {
                            weekday: 'short',
                            year: 'numeric',
                            month: 'long',
                            day: 'numeric',
                            hour: '2-digit',
                            minute: '2-digit'
                        })}</p>
                    </div>
                </div>

                <div className="flex-2 flex justify-center items-center min-w-0">
                    <div
                        className={`h-full [writing-mode:vertical-lr] rotate-180 text-center ${mapModuleStatusToColor(report.module_status.state)}`}>
                        <h2 className="font-black">{report.module_status.state.toUpperCase()}</h2>
                    </div>
                </div>
            </div>
        </div>
    )
}


