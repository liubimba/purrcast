package http

import (
	"net/http"
	"os"
	"path/filepath"
)

func SpaHandler(staticPath string, indexFile string) http.Handler {
	fs := http.FileServer(http.Dir(staticPath))

	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		path := filepath.Join(staticPath, r.URL.Path)

		info, err := os.Stat(path)
		if err == nil && !info.IsDir() {
			fs.ServeHTTP(w, r)
			return
		}
		http.ServeFile(w, r, filepath.Join(staticPath, indexFile))
	})
}
