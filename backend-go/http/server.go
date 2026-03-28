package http

import (
	"fmt"
	"log"
	"multiroom/backend-go/http/api/config"
	"multiroom/backend-go/http/websocket"
	"multiroom/backend-go/hub"
	"net/http"
)

func StartHTTPServer(httpPort int, monitorPort int, host string, externalAddress string, staticDir string) {
	http.Handle("/", SpaHandler(staticDir, "index.html"))

	h := hub.NewHub(httpPort, monitorPort, fmt.Sprintf("%s.local.", host), externalAddress)
	go h.Run()

	http.HandleFunc("/api/config", config.ConfigHandler(h.Configuration))
	http.HandleFunc("/ws", websocket.UpgradeHandler(h))

	listenAddress := fmt.Sprintf(":%d", httpPort)
	log.Printf("Listening on %s\n", listenAddress)

	if err := http.ListenAndServe(listenAddress, nil); err != nil {
		log.Fatal(err)
	}
}
