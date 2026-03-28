package mdns

import (
	"log"
	"net"

	"github.com/grandcat/zeroconf"
)

func RegisterMDNS(iface net.Interface, ip net.IP, httpPort int) {
	server, err := zeroconf.RegisterProxy(
		"multiroom",
		"_http._tcp",
		"local.",
		httpPort,
		"multiroom",
		[]string{ip.String()},
		nil,
		[]net.Interface{iface},
	)
	if err != nil {
		log.Fatal(err)
	}
	defer server.Shutdown()
	log.Printf("mDNS service running: multiroom.local:%d", httpPort)
}
