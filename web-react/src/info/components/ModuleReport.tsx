import * as React from "react";
import type {ModuleReportEntity} from "../entity/ModuleReportEntity.ts";
import "../styles/moduleReport.css"

export const ModuleReport: React.FC<{ report: ModuleReportEntity }> = ({report}) => {
    const date = new Date(Number.parseInt(report.module_status.timestamp));

    const mapModuleStatusToColor = (status: string) => {
        if (status.toLowerCase() === "running") {
            return "var(--color-success)";
        } else if (status.toLowerCase() === "failed") {
            return "var(--color-error)";
        }
        return "#4B4B4B";
    }

    return (
        <div className="module-report-container ">
            <div className="flex module-report-wrapper bg-primary">
                <div className="flex flex-col flex-3">
                    <div className="flex-2 flex items-center px-4">
                        <h2 style={{fontSize: "1.4rem"}} className="font-black">{report.module_name}</h2>
                    </div>
                    <div className="flex-1 flex items-end border-t-2 py-2 px-4">
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
                <div className="flex-1  flex items-center justify-center min-w-0 min-h-0">
                    <div
                        className=" p-2 text-center break-words max-w-full max-h-full h-full w-full min-h-0"
                        style={{background: mapModuleStatusToColor(report.module_status.state)}}>
                        <h2 className="h-full font-black rotate-180" style={{
                            fontSize: "1.4rem",
                            writingMode: "vertical-rl",
                            textOrientation: "mixed",
                        }}>
                            {report.module_status.state.toUpperCase()}
                        </h2>
                    </div>
                </div>
            </div>
        </div>

    )
}


