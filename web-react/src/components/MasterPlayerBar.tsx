import {useMasterPlayer} from "../hooks/useMasterPlayer.ts";

interface PlayerBarProps {
}


export const MasterPlayerBar: React.FC<PlayerBarProps> = () => {
    const {masterVolume, masterMuted, updateMasterVolume, updateMasterMuted} = useMasterPlayer();

    const onMutedToggle = (): void => {
        updateMasterMuted(!masterMuted);
    }

    const onVolumeChanged: React.ChangeEventHandler<HTMLInputElement> = (event: React.ChangeEvent<HTMLInputElement>): void => {
        updateMasterVolume(Number(event.target.value));
    }

    return (
        <section style={{gridArea: "playerbar"}}>
            <div className="border-2 h-full flex flex-col">
                <div className="w-full flex justify-center flex-1">
                    <button onClick={onMutedToggle}>
                        {masterMuted ? "Stop" : "Pause"}
                    </button>
                </div>

                <div>
                    <input value={masterVolume}
                           onChange={onVolumeChanged}
                           type="range" min={0} max={100} className="w-full flex-auto my-5"/>
                </div>
            </div>
        </section>

    )
}
