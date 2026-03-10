import {TestControl} from "./Control.tsx";

export const Controls: React.FC = () => {
    return (
        <div className="flex flex-wrap gap-8">

            <TestControl/>
            <TestControl/>
            <TestControl/>
            <TestControl/>
            <TestControl/>
            <TestControl/>
        </div>
    )
}