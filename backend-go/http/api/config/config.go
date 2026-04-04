package config

import (
	"encoding/json"
	"multiroom/backend-go/hub"
	"net/http"
)

func ConfigHandler(configuration hub.Configuration) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		w.Header().Set("Access-Control-Allow-Origin", "*")
		err := json.NewEncoder(w).Encode(configuration)
		if err != nil {
			return
		}
	}
}
