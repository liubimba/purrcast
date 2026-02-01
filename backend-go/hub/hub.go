package hub

func NewHub(listenPort int) *Hub {
	return &Hub{
		Clients:   make(map[*Client]bool),
		Broadcast: make(chan Envelope),
		Configuration: Configuration{
			Master: MasterPlayerState{
				Volume: 50,
				Muted:  false,
			},
			Snapserver: SnapserverConfig{
				Ports: SnapserverPorts{
					Http:    1780,
					Stream:  1704,
					Control: 1705,
				},
			},
			Websocket: WebsocketConfig{
				Path: "/ws",
				Port: listenPort,
			},
			Monitor: MonitorConfig{
				Port: 3000,
			},
		},
	}
}

// Run continuously broadcasts messages to all clients except sender
func (h *Hub) Run() {
	for env := range h.Broadcast {
		for c := range h.Clients {
			if c == env.Sender {
				continue
			}
			c.Send(env.Data)
		}
	}
}
