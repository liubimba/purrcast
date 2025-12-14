// eslint-disable-next-line @typescript-eslint/ban-ts-comment
// @ts-ignore
namespace Snapcast {
    export interface Host {
        arch: string;
        ip: string;
        mac: string;
        name: string;
        os: string;
    }


    export interface Client {
        id: string;
        host: Host;
        snapclient: {
            name: string;
            protocolVersion: number;
            version: string;
        }
        config: {
            instance: number;
            latency: number;
            name: string;
            volume: {
                muted: boolean;
                percent: number;
            }
        };
        lastSeen: {
            sec: number;
            usec: number;
        };
        connected: boolean;
    }


    export interface Group {
        name: string;
        id: string;
        stream_id: string;
        muted: boolean;
        clients: Client[];
    }

    export interface Metadata {
        title?: string;
        artist?: string[];
        album?: string;
        artUrl?: string;
        duration?: number;
    }

    type PlaybackStatus = 'stopped' | 'paused' | 'playing';

    export interface Properties {
        loopStatus?: string;
        shuffle?: boolean
        volume?: number;
        rate?: number;
        playbackStatus?: PlaybackStatus;
        position?: number;
        minimumRate?: number;
        maximumRate?: number;
        canGoNext: boolean;
        canGoPrevious: boolean;
        canPlay: boolean;
        canPause: boolean;
        canSeek: boolean;
        canControl: boolean;
        metadata?: Metadata;
    }

    export interface Stream {
        id: string;
        status: string;
        uri: {
            raw: string;
            scheme: string;
            host: string;
            path: string;
            fragment: string;
            query: string;
        }

        properties: Properties;
    }


    export interface Server {
        groups: Group[];
        server: {
            host: Host;
            snapserver: {
                controlProtocolVersion: number;
                name: string;
                protocolVersion: number;
                version: string;
            }
        };
        streams: Stream[];
    }

    export class Mapper {
        public static toHost(json: any): Host {
            return {
                arch: json ? json.arch : "",
                ip: json ? json.ip : "",
                mac: json ? json.mac : "",
                name: json ? json.name : "",
                os: json ? json.os : "",
            }
        }

        public static toClient(json: any): Client {
            return {
                id: json.id,
                host: Mapper.toHost(json.host),
                snapclient: {
                    name: json.snapclient.name,
                    protocolVersion: json.snapclient.protocolVersion,
                    version: json.snapclient.version
                },
                config: {
                    instance: json.config.instance,
                    latency: json.config.latency,
                    name: json.config.name,
                    volume: {muted: json.config.volume.muted, percent: json.config.volume.percent}
                },
                lastSeen: {sec: json.lastSeen.sec, usec: json.lastSeen.usec},
                connected: Boolean(json.connected),
            }
        }

        public static toGroup(json: any): Group {
            return {
                name: json.name,
                id: json.id,
                stream_id: json.stream_id,
                muted: Boolean(json.muted),
                clients: json.clients.map((c: any) => Mapper.toClient(c))
            }
        }

        public static toMetadata(json: any): Metadata {
            return {
                title: json ? json.title : "",
                artist: json ? json.artist : null,
                album: json ? json.album : null,
                artUrl: json ? json.artUrl : null,
                duration: json ? json.duration : null,
            }
        }

        public static toProperties(json: any): Properties {
            return {
                loopStatus: json.loopStatus,
                shuffle: json.shuffle,
                volume: json.volume,
                rate: json.rate,
                playbackStatus: json.playbackStatus,
                position: json.position,
                minimumRate: json.minimumRate,
                maximumRate: json.maximumRate,
                canGoNext: Boolean(json.canGoNext),
                canGoPrevious: Boolean(json.canGoPrevious),
                canPlay: Boolean(json.canPlay),
                canPause: Boolean(json.canPause),
                canSeek: Boolean(json.canSeek),
                canControl: Boolean(json.canControl),
                metadata: json.metadata ?? Mapper.toMetadata(json.metadata),
            }
        }

        public static toStream(json: any): Stream {
            return {
                id: json.id,
                status: json.status,
                properties: json.properties ? Mapper.toProperties(json.properties) : Mapper.toProperties({}),
                uri: {
                    raw: json.uri.raw,
                    scheme: json.uri.scheme,
                    host: json.uri.host,
                    path: json.uri.path,
                    fragment: json.uri.fragment,
                    query: json.uri.query
                }
            }
        }

        public static toServer(json: any): Server {
            return {
                groups: json.groups.map((group: any) => Mapper.toGroup(group)),
                server: {
                    host: Mapper.toHost(json.server.host),
                    snapserver: {
                        controlProtocolVersion: json.server.snapserver.controlProtocolVersion,
                        name: json.server.snapserver.name,
                        protocolVersion: json.server.snapserver.protocolVersion,
                        version: json.server.snapserver.version
                    }
                },
                streams: json.streams.map((stream: any) => Mapper.toStream(stream)),
            }
        }
    }

    export const getDefaultServer = () => {
        return {
            groups: [],
            server: {
                host: {
                    arch: "",
                    ip: "",
                    mac: "",
                    name: "",
                    os: ""
                },
                snapserver: {
                    controlProtocolVersion: 0,
                    name: "",
                    protocolVersion: 0,
                    version: ""
                }
            },
            streams: []
        }
    };

}

// interface OnChange { (_server: Snapcast.Server): void }
// interface OnStreamChange { (id: string): void };

class SnapControl {
    constructor() {
        this.onChange = null;
        this.onConnectionChanged = null;
        this.server = Snapcast.getDefaultServer();
        this.msg_id = 0;
        this.status_req_id = -1;
        this.timer = null;
    }

    public connect(baseUrl: string) {
        this.disconnect();
        try {
            const jsonRPCUrl = new URL(baseUrl);
            jsonRPCUrl.pathname = "/jsonrpc";
            this.connection = new WebSocket(jsonRPCUrl);
            this.connection.onmessage = (msg: MessageEvent) => this.onMessage(msg.data);
            this.connection.onopen = () => {
                this.status_req_id = this.sendRequest('Server.GetStatus');
                if (this.onConnectionChanged)
                    this.onConnectionChanged(this, true);
            };
            this.connection.onerror = (ev: Event) => {
                console.error('error:', ev);
            };
            this.connection.onclose = () => {
                if (this.onConnectionChanged)
                    this.onConnectionChanged(this, false, 'Connection lost, trying to reconnect.');
                console.info('connection lost, reconnecting in 1s');
                this.timer = setTimeout(() => this.connect(baseUrl), 1000);
            };
        } catch (e) {
            console.error('Exception while connecting: "' + e + '", reconnecting in 1s');
            if (this.onConnectionChanged)
                this.onConnectionChanged(this, false, 'Exception while connecting: "' + e + '", trying to reconnect.');
            this.timer = setTimeout(() => this.connect(baseUrl), 1000);
        }
    }

    public disconnect() {
        if (this.timer)
            clearTimeout(this.timer);
        if (this.connection) {
            this.connection.onclose = () => {
            };
            if (this.connection.readyState === WebSocket.OPEN) {
                this.connection.close();
            }
        }
        if (this.onConnectionChanged)
            this.onConnectionChanged(this, false);
    }

    onChange: ((_this: SnapControl, _server: Snapcast.Server) => any) | null;
    onConnectionChanged: ((_this: SnapControl, _connected: boolean, _error?: string) => any) | null;

    private onNotification(notification: any): boolean {
        switch (notification.method) {
            case 'Client.OnVolumeChanged':
                this.getClient(notification.params.id).config.volume = notification.params.volume;
                // updateGroupVolume(this.getGroupFromClient(client.id));
                return true;
            case 'Client.OnLatencyChanged':
                this.getClient(notification.params.id).config.latency = notification.params.latency;
                return false;
            case 'Client.OnNameChanged':
                this.getClient(notification.params.id).config.name = notification.params.name;
                return true;
            case 'Client.OnConnect':
            case 'Client.OnDisconnect': {
                let indexOfGroup = -1;
                let indexOfClient = -1;
                this.server.groups.forEach((group: Snapcast.Group, groupIndex) => {
                    group.clients.forEach((client: Snapcast.Client, clientIndex) => {
                        if (client.id === notification.params.client.id) {
                            indexOfGroup = groupIndex;
                            indexOfClient = clientIndex;
                        }
                    })
                })
                if (indexOfClient === -1) {
                    throw new Error("Client does not exist");
                }
                this.server.groups[indexOfGroup].clients[indexOfClient] = Snapcast.Mapper.toClient(notification.params.client);
                return true;
            }
            case 'Group.OnMute':
                this.getGroup(notification.params.id).muted = Boolean(notification.params.mute);
                return true;
            case 'Group.OnStreamChanged':
                this.getGroup(notification.params.id).stream_id = notification.params.stream_id;
                return true;
            case 'Stream.OnUpdate': {
                const indexOfStream = this.server.streams.findIndex(stream => stream.id === notification.params.id);
                if (indexOfStream === -1) {
                    throw new Error("");
                }
                this.server.streams[indexOfStream] = Snapcast.Mapper.toStream(notification.params.stream);
                return true;
            }
            case 'Server.OnUpdate':
                this.server = Snapcast.Mapper.toServer(notification.params.server);
                return true;
            case 'Stream.OnProperties': {
                const indexOfStream = this.server.streams.findIndex(stream => stream.id === notification.params.id);
                if (indexOfStream === -1) {
                    throw new Error("");
                }
                this.server.streams[indexOfStream].properties = Snapcast.Mapper.toProperties(notification.params.properties);
                return true;
            }
            default:
                return false;
        }
    }

    public getClient(client_id: string): Snapcast.Client {
        const group = this.server.groups.find(group => group.clients.find(client => client.id === client_id));
        if (!group) {
            throw new Error(`client ${client_id} was null`);
        }
        return group.clients.find(client => client.id === client_id)!;
    }

    public getGroup(group_id: string): Snapcast.Group {
        const group = this.server.groups.find(group => group.id === group_id);
        if (group == null) {
            throw new Error(`group ${group_id} was null`);
        }
        return group;
    }

    public getGroupVolume(group: Snapcast.Group, online: boolean): number {
        if (group.clients.length === 0)
            return 0;
        let group_vol: number = 0;
        let client_count: number = 0;
        for (const client of group.clients) {
            if (online && !client.connected)
                continue;
            group_vol += client.config.volume.percent;
            ++client_count;
        }
        if (client_count === 0)
            return 0;
        return group_vol / client_count;
    }

    public getGroupFromClient(client_id: string): Snapcast.Group {
        for (const group of this.server.groups)
            for (const client of group.clients)
                if (client.id === client_id)
                    return group;
        throw new Error(`group for client ${client_id} was null`);
    }

    public getStreamFromClient(client_id: string): Snapcast.Stream {
        const group: Snapcast.Group = this.getGroupFromClient(client_id);
        return this.getStream(group.stream_id);
    }

    public getStream(stream_id: string): Snapcast.Stream {
        const stream = this.server.streams.find(stream => stream.id === stream_id);
        if (stream == null) {
            throw new Error(`stream ${stream_id} was null`);
        }
        return stream;
    }

    public setVolume(client_id: string, percent: number, mute?: boolean) {
        percent = Math.max(0, Math.min(100, percent));
        const client = this.getClient(client_id);
        client.config.volume.percent = percent;
        if (mute !== undefined)
            client.config.volume.muted = mute;
        this.sendRequest('Client.SetVolume', {
            id: client_id,
            volume: {muted: client.config.volume.muted, percent: client.config.volume.percent}
        });
    }

    public setClientName(client_id: string, name: string) {
        const client = this.getClient(client_id);
        const current_name: string = (client.config.name !== "") ? client.config.name : client.host.name;
        if (name !== current_name) {
            this.sendRequest('Client.SetName', {id: client_id, name: name});
            client.config.name = name;
        }
    }

    public setClientLatency(client_id: string, latency: number) {
        const client = this.getClient(client_id);
        const current_latency: number = client.config.latency;
        if (latency !== current_latency) {
            this.sendRequest('Client.SetLatency', {id: client_id, latency: latency});
            client.config.latency = latency;
        }
    }

    public deleteClient(client_id: string) {
        this.sendRequest('Server.DeleteClient', {id: client_id});
        this.server.groups.forEach((g: Snapcast.Group, gi: number) => {
            g.clients.forEach((c: Snapcast.Client, ci: number) => {
                if (c.id === client_id) {
                    this.server.groups[gi].clients.splice(ci, 1);
                }
            })
        })

        this.server.groups.forEach((g: Snapcast.Group, gi: number) => {
            if (g.clients.length === 0) {
                this.server.groups.splice(gi, 1);
            }
        });
        // show();
    }

    public setStream(group_id: string, stream_id: string) {
        this.getGroup(group_id).stream_id = stream_id;
        this.sendRequest('Group.SetStream', {id: group_id, stream_id: stream_id});
    }

    public setClients(group_id: string, clients: string[]) {
        this.status_req_id = this.sendRequest('Group.SetClients', {id: group_id, clients: clients});
    }

    public muteGroup(group_id: string, mute: boolean) {
        this.getGroup(group_id).muted = mute;
        this.sendRequest('Group.SetMute', {id: group_id, mute: mute});
    }

    public control(stream_id: string, command: string, params?: any) {
        const json: any = {id: stream_id, command: command};
        if (params) {
            json.params = params;
        }
        this.sendRequest('Stream.Control', json);
    }

    private sendRequest(method: string, params?: any): number {
        const msg: any = {
            id: ++this.msg_id,
            jsonrpc: '2.0',
            method: method
        };
        if (params)
            msg.params = params;

        const msgJson = JSON.stringify(msg);
        console.debug("Sending: " + msgJson);
        this.connection.send(msgJson);
        return this.msg_id;
    }

    private onMessage(msg: string) {
        let refresh: boolean = false;
        const json_msg = JSON.parse(msg);
        const is_response: boolean = (json_msg.id !== undefined);
        // console.debug("Received " + (is_response ? "response" : "notification") + ", json: " + JSON.stringify(json_msg))
        if (is_response) {
            if (json_msg.id === this.status_req_id) {
                this.server = Snapcast.Mapper.toServer(json_msg.result.server);
                refresh = true;
            }
        } else {
            if (Array.isArray(json_msg)) {
                for (const notification of json_msg) {
                    refresh = this.onNotification(notification) || refresh;
                }
            } else {
                refresh = this.onNotification(json_msg);
            }
            refresh = true;

            // TODO: don't update everything, but only the changed, 
            // e.g. update the values for the volume sliders
            // if (refresh)
            //     show();
        }
        if (refresh) {
            if (this.onChange) {
                this.onChange(this, this.server);
            } else {
            }
        }
    }

    // public onChange?: OnChange;
    // public onStreamChange?: OnStreamChange;

    connection!: WebSocket;
    server: Snapcast.Server;
    msg_id: number;
    status_req_id: number;
    timer: ReturnType<typeof setTimeout> | null;
}


export {SnapControl}
export {Snapcast}
