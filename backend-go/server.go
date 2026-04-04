package main

import (
	"flag"
	"log"
	"multiroom/backend-go/http"
	"multiroom/backend-go/mdns"
	"multiroom/backend-go/utility"
	"os"
	"path/filepath"
)

func main() {
	httpPort := flag.Int("http.port", -1, "Port to listen on HTTP")
	staticDir := flag.String("http.static_dir", "", "Directory with web-react static files")
	monitorPort := flag.Int("monitor.port", -1, "Monitor port to listen")

	flag.Parse()
	if *httpPort <= 0 {
		log.Fatal("--http.port must be specified")
	}
	if *staticDir == "" {
		log.Fatal("--http.static_dir must be specified")
	}
	absStaticDir, err := filepath.Abs(*staticDir)
	if err != nil {
		log.Fatalf("Failed to resolve static_dir: %v", err)
	}
	info, err := os.Stat(absStaticDir)
	if err != nil || !info.IsDir() {
		log.Fatalf("static_dir is not a directory: %s", absStaticDir)
	}

	host := "multiroom"
	iface, ip := utility.GetExternalInterface()
	mdns.RegisterMDNS(iface, ip, *httpPort)
	http.StartHTTPServer(*httpPort, *monitorPort, host, ip.String(), absStaticDir)
}
