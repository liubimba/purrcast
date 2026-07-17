package config

import (
	"encoding/json"
	"multiroom/backend-go/hub"
	"net/http"
	"net/http/httptest"
	"testing"
)

func configuration() hub.Configuration {
	return hub.Configuration{
		Master:     hub.MasterPlayerState{Volume: 42, Muted: true},
		Snapserver: hub.SnapserverConfig{Ports: hub.SnapserverPorts{Http: 1780, Stream: 1704, Control: 1705}},
		Websocket:  hub.WebsocketConfig{Path: "/ws", Port: 5000},
		Monitor:    hub.MonitorConfig{Port: 5001},
		Host:       hub.HostConfig{Hostname: "multiroom", Address: "192.168.0.10", Port: 5000},
	}
}

func serve(t *testing.T) *httptest.ResponseRecorder {
	t.Helper()
	recorder := httptest.NewRecorder()
	ConfigHandler(configuration()).ServeHTTP(recorder, httptest.NewRequest(http.MethodGet, "/api/config", nil))
	return recorder
}

func TestServesTheConfigurationAsJSON(t *testing.T) {
	recorder := serve(t)

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusOK)
	}
	if got := recorder.Header().Get("Content-Type"); got != "application/json" {
		t.Errorf("Content-Type = %q, want %q", got, "application/json")
	}

	var decoded hub.Configuration
	if err := json.Unmarshal(recorder.Body.Bytes(), &decoded); err != nil {
		t.Fatalf("body is not valid JSON: %v", err)
	}
	if decoded != configuration() {
		t.Errorf("decoded = %+v, want %+v", decoded, configuration())
	}
}

func TestBrowserOnAnotherOriginMayReadTheConfiguration(t *testing.T) {
	if got := serve(t).Header().Get("Access-Control-Allow-Origin"); got != "*" {
		t.Errorf("Access-Control-Allow-Origin = %q, want %q", got, "*")
	}
}

func TestPortsSurviveTheRoundTripUnderTheNamesTheClientReads(t *testing.T) {
	var decoded map[string]any
	if err := json.Unmarshal(serve(t).Body.Bytes(), &decoded); err != nil {
		t.Fatalf("body is not valid JSON: %v", err)
	}

	snapserver, ok := decoded["snapserver"].(map[string]any)
	if !ok {
		t.Fatalf("no snapserver object in %v", decoded)
	}
	ports, ok := snapserver["ports"].(map[string]any)
	if !ok {
		t.Fatalf("no ports object in %v", snapserver)
	}
	for name, want := range map[string]float64{"http": 1780, "stream": 1704, "control": 1705} {
		if ports[name] != want {
			t.Errorf("ports[%q] = %v, want %v", name, ports[name], want)
		}
	}
}
