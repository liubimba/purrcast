package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"net"
	"net/http"
	"os"
	"path/filepath"

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

type UserDataPayload struct {
	IsLocal bool `json:"is_local"`
}

type UserDataMessage struct {
	Type    string          `json:"type"`
	Payload UserDataPayload `json:"payload"`
}

func isLocalConnection(r *http.Request) bool {
	host, _, err := net.SplitHostPort(r.RemoteAddr)
	if err != nil {
		return false
	}
	remoteIP := net.ParseIP(host)
	if remoteIP == nil {
		return false
	}
	if remoteIP.IsLoopback() {
		return true
	}
	ifaces, err := net.Interfaces()
	if err != nil {
		return false
	}
	for _, iface := range ifaces {
		addrs, err := iface.Addrs()
		if err != nil {
			continue
		}
		for _, addr := range addrs {
			var ip net.IP
			switch v := addr.(type) {
			case *net.IPNet:
				ip = v.IP
			case *net.IPAddr:
				ip = v.IP
			}
			if ip != nil && ip.Equal(remoteIP) {
				return true
			}
		}
	}
	return false
}

func (h *Hub) upgradeHandler(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		return
	}
	h.clients[conn] = true

	msg := UserDataMessage{
		Type: "USER_DATA",
		Payload: UserDataPayload{
			IsLocal: isLocalConnection(r),
		},
	}

	if err := conn.WriteJSON(msg); err != nil {
		log.Println("write error:", err)
		conn.Close()
		return
	}

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

func makeConfigHandler(listenPort int) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

		cfg := ConfigResponse{
			SnapserverConfig: SnapserverConfig{
				Ports: SnapserverPorts{Http: 1780},
			},
			WebsocketConfig: WebsocketConfig{
				Path: "/ws",
				Port: listenPort,
			},
		}

		w.Header().Set("Content-Type", "application/json")
		w.Header().Set("Access-Control-Allow-Origin", "*")
		json.NewEncoder(w).Encode(cfg)
	}
}

func spaHandler(staticPath string, indexFile string) http.Handler {
	fs := http.FileServer(http.Dir(staticPath))

	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		path := filepath.Join(staticPath, r.URL.Path)

		info, err := os.Stat(path)
		if err == nil && !info.IsDir() {
			fs.ServeHTTP(w, r)
			return
		}
		http.ServeFile(w, r, filepath.Join(staticPath, indexFile))
	})
}

func main() {
	port := flag.Int("port", -1, "Port to listen on")
	staticDir := flag.String("static_dir", "", "Directory with web-react static files")
	flag.Parse()
	if *port <= 0 {
		log.Fatal("--port must be specified")
	}
	if *staticDir == "" {
		log.Fatal("--static_dir must be specified")
	}
	absStaticDir, err := filepath.Abs(*staticDir)
	if err != nil {
		log.Fatalf("Failed to resolve static_dir: %v", err)
	}
	info, err := os.Stat(absStaticDir)
	if err != nil || !info.IsDir() {
		log.Fatalf("static_dir is not a directory: %s", absStaticDir)
	}

	addr := fmt.Sprintf(":%d", *port)

	http.Handle("/", spaHandler(absStaticDir, "index.html"))

	hub := newHub()
	go hub.run()

	http.HandleFunc("/api/config", makeConfigHandler(*port))
	http.HandleFunc("/ws", hub.upgradeHandler)

	log.Printf("Listening on %s\n", addr)

	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatal(err)
	}
}
