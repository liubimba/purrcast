import {useSelector} from "react-redux";
import type {RootState} from "../../store/store.ts";
import type {ModuleReport} from "../module/moduleReport.ts";

export const useMonitor = () => {
    const reports: ModuleReport[] = useSelector((state: RootState) => state.monitor.reports);
    return {
        reports: reports
    }
}