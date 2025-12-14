import {Snapcast} from "../snapcontrol.ts";

interface StreamsListProps {
    server: Snapcast.Server;
    onSelectStream?: (streamId: string) => void;
}

export const StreamsList = (props: StreamsListProps) => {
    return (
        <div>
            <select onChange={event => {
                if (props.onSelectStream) {
                    props.onSelectStream(event.target.value);
                }
            }}>
                {props.server.streams.map((stream: Snapcast.Stream) => (
                    <option key={stream.id} value={stream.id}>{stream.id}</option>
                ))}
            </select>
        </div>
    )
}