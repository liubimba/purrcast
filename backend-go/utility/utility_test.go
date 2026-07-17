package utility

import (
	"net/http"
	"net/http/httptest"
	"testing"
)

func requestFrom(remoteAddr string) *http.Request {
	request := httptest.NewRequest(http.MethodGet, "/ws", nil)
	request.RemoteAddr = remoteAddr
	return request
}

func TestLoopbackCountsAsLocal(t *testing.T) {
	for _, remoteAddr := range []string{"127.0.0.1:54321", "[::1]:54321"} {
		if !IsLocalConnection(requestFrom(remoteAddr)) {
			t.Errorf("IsLocalConnection(%q) = false, want true", remoteAddr)
		}
	}
}

func TestAnAddressOffThisMachineIsNotLocal(t *testing.T) {
	if IsLocalConnection(requestFrom("203.0.113.7:54321")) {
		t.Error("a documentation-range address was treated as local")
	}
}

func TestUnreadableRemoteAddressIsNotLocal(t *testing.T) {
	for _, remoteAddr := range []string{"", "not-an-address", "127.0.0.1", "no-port:", "abc:54321"} {
		if IsLocalConnection(requestFrom(remoteAddr)) {
			t.Errorf("IsLocalConnection(%q) = true, want false", remoteAddr)
		}
	}
}
