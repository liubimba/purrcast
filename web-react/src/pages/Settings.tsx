import * as React from "react";

interface SettingsProps {

}

export const Settings: React.FC<SettingsProps> = ({}) => {
    return (
        <div>
            <form>
                <fieldset>
                    <legend>Snapserver</legend>
                    <label htmlFor="settings-input">Base URL</label>
                </fieldset>
            </form>

        </div>
    )
}