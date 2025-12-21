package main

import (
	"flag"
	"fmt"
	"log"
	mlConfig "multiroom/backend/api/config"
	mlHttp "multiroom/backend/http"
	"multiroom/backend/hub"
	mlWebsocket "multiroom/backend/websocket"
	"net/http"
	"os"
	"path/filepath"
)

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

	http.Handle("/", mlHttp.SpaHandler(absStaticDir, "index.html"))

	h := hub.NewHub(*port)
	go h.Run()

	http.HandleFunc("/api/config", mlConfig.ConfigHandler(h.Configuration))
	http.HandleFunc("/ws", mlWebsocket.UpgradeHandler(h))

	log.Printf("Listening on %s\n", addr)

	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatal(err)
	}
}
