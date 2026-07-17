package http

import (
	"net/http"
	"net/http/httptest"
	"os"
	"path/filepath"
	"testing"
)

func staticDirectory(t *testing.T) string {
	t.Helper()
	directory := t.TempDir()
	write(t, filepath.Join(directory, "index.html"), "<!doctype html>index")
	write(t, filepath.Join(directory, "app.js"), "console.log(1)")
	return directory
}

func write(t *testing.T, path string, body string) {
	t.Helper()
	if err := os.WriteFile(path, []byte(body), 0o600); err != nil {
		t.Fatalf("cannot write %s: %v", path, err)
	}
}

func get(t *testing.T, directory string, path string) *httptest.ResponseRecorder {
	t.Helper()
	recorder := httptest.NewRecorder()
	SpaHandler(directory, "index.html").ServeHTTP(recorder, httptest.NewRequest(http.MethodGet, path, nil))
	return recorder
}

func TestServesAFileThatExists(t *testing.T) {
	recorder := get(t, staticDirectory(t), "/app.js")

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusOK)
	}
	if recorder.Body.String() != "console.log(1)" {
		t.Errorf("body = %q, want the file's own contents", recorder.Body.String())
	}
}

func TestFallsBackToIndexSoTheClientCanRouteItsOwnPaths(t *testing.T) {
	recorder := get(t, staticDirectory(t), "/about")

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusOK)
	}
	if recorder.Body.String() != "<!doctype html>index" {
		t.Errorf("body = %q, want index.html", recorder.Body.String())
	}
}

func TestFallsBackToIndexForADirectory(t *testing.T) {
	directory := staticDirectory(t)
	if err := os.Mkdir(filepath.Join(directory, "assets"), 0o700); err != nil {
		t.Fatalf("cannot create directory: %v", err)
	}

	if body := get(t, directory, "/assets").Body.String(); body != "<!doctype html>index" {
		t.Errorf("body = %q, want index.html", body)
	}
}

func TestDoesNotServeFilesAboveTheStaticDirectory(t *testing.T) {
	directory := staticDirectory(t)
	write(t, filepath.Join(filepath.Dir(directory), "secret.txt"), "not yours")

	recorder := get(t, directory, "/../secret.txt")

	if recorder.Body.String() == "not yours" {
		t.Fatal("served a file from outside the static directory")
	}
}
