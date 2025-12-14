package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"net/http"

	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool { return true },
}

type Envelope struct {
	Sender *websocket.Conn
	Data   []byte
}
type Hub struct {
	clients   map[*websocket.Conn]bool
	broadcast chan Envelope
}

func newHub() *Hub {
	return &Hub{
		clients:   make(map[*websocket.Conn]bool),
		broadcast: make(chan Envelope),
	}
}

func (h *Hub) run() {
	for env := range h.broadcast {
		for c := range h.clients {
			if c == env.Sender {
				log.Println("Sender is already in hub")
				continue
			}
			log.Println("Sent")
			c.WriteMessage(websocket.TextMessage, env.Data)
		}
	}
}

func (h *Hub) upgradeHandler(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		return
	}
	h.clients[conn] = true
	log.Println("new connection %s", conn.LocalAddr())
	go func() {
		defer conn.Close()
		for {
			_, msg, err := conn.ReadMessage()
			if err != nil {
				delete(h.clients, conn)
				return
			}
			log.Printf("client %s sent: %s\n", conn.LocalAddr(), msg)
			h.broadcast <- Envelope{Sender: conn, Data: msg}
		}
	}()
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
type ConfigResponse struct {
	SnapserverConfig SnapserverConfig `json:"snapserver"`
	WebsocketConfig  WebsocketConfig  `json:"websocket"`
}

func configHandler(w http.ResponseWriter, r *http.Request) {
	cfg := ConfigResponse{
		SnapserverConfig: SnapserverConfig{
			Ports: SnapserverPorts{Http: 1780},
		},
		WebsocketConfig: WebsocketConfig{
			Path: "/ws",
			Port: 8080,
		},
	}
	w.Header().Set("Content-Type", "application/json")
	w.Header().Set("Access-Control-Allow-Origin", "*")
	err := json.NewEncoder(w).Encode(cfg)
	if err != nil {
		return
	}
}

func main() {
	port := flag.Int("port", 8080, "Port to listen on")
	flag.Parse()

	addr := fmt.Sprintf(":%d", *port)

	hub := newHub()
	go hub.run()

	http.HandleFunc("/api/config", configHandler)
	http.HandleFunc("/ws", hub.upgradeHandler)

	log.Printf("Listening on %s\n", addr)

	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatal(err)
	}
}
