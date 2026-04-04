package utility

import (
	"net"
	"net/http"
)

func GetExternalInterface() (net.Interface, net.IP) {
	interfaces, _ := net.Interfaces()

	for _, iface := range interfaces {

		if iface.Flags&net.FlagUp == 0 {
			continue
		}

		if iface.Flags&net.FlagMulticast == 0 {
			continue
		}

		addrs, _ := iface.Addrs()

		for _, addr := range addrs {

			ipnet, ok := addr.(*net.IPNet)
			if !ok {
				continue
			}

			ip := ipnet.IP.To4()
			if ip == nil || ip.IsLoopback() {
				continue
			}

			return iface, ip
		}
	}
	return net.Interface{}, nil
}

func IsLocalConnection(r *http.Request) bool {
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
