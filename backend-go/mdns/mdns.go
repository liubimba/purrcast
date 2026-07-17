package mdns

import (
	"log"
	"net"

	"github.com/grandcat/zeroconf"
)

func RegisterMDNS(iface net.Interface, ip net.IP, httpPort int) (*zeroconf.Server, error) {
	server, err := zeroconf.RegisterProxy(
		"purrcast",
		"_http._tcp",
		"local.",
		httpPort,
		"purrcast",
		[]string{ip.String()},
		nil,
		[]net.Interface{iface},
	)
	if err != nil {
		return nil, err
	}
	log.Printf("mDNS service running: purrcast.local:%d", httpPort)
	return server, nil
}
