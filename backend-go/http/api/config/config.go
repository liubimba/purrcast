package config

import (
	"encoding/json"
	"net/http"
	"purrcast/backend-go/hub"
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
