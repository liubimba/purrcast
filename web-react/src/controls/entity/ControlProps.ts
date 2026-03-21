export interface ControlProps {
    volume: number,
    muted: boolean,
    name: string,
    index: number,
    className?: string,
    onVolumeChange?: (volume: number) => void,
    onMutedChange?: (muted: boolean) => void,
}