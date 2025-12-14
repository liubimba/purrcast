interface Props {
    volume: number
    onChange: (value: number) => void
}

export const VolumeSlider = (props: Props) => {
    return (
        <div>
            <input type="range" min={0} max={100} value={props.volume}
                   onChange={e => props.onChange(Number(e.target.value))}/>
            <span>{props.volume}</span>
        </div>
    )

}