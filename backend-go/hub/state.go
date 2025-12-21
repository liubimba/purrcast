package hub

import (
	"sync"
)

// MasterPlayerState holds the current state of the master player
type MasterPlayerState struct {
	Volume int32 `json:"volume"`
	Muted  bool  `json:"muted"`
}

// Envelope contains sender and raw data
type Envelope struct {
	Sender *Client
	Data   []byte
}

type SnapserverPorts struct {
	Http    int `json:"http"`
	Stream  int `json:"stream"`
	Control int `json:"control"`
}
type SnapserverConfig struct {
	Ports SnapserverPorts `json:"ports"`
}

type WebsocketConfig struct {
	Path string `json:"path"`
	Port int    `json:"port"`
}

type Configuration struct {
	Master     MasterPlayerState `json:"masterPlayer"`
	Snapserver SnapserverConfig  `json:"snapserver"`
	Websocket  WebsocketConfig   `json:"websocket"`
}

// Hub holds connected clients and broadcast channel
type Hub struct {
	Mutex         sync.RWMutex
	Clients       map[*Client]bool
	Broadcast     chan Envelope
	Configuration Configuration
}
