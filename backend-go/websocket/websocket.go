package websocket

import (
	"encoding/json"
	"multiroom/backend/hub"
	"multiroom/backend/message"
	"net/http"

	"github.com/gorilla/websocket"
)

func UpgradeHandler(h *hub.Hub) http.HandlerFunc {
	var upgrader = websocket.Upgrader{
		CheckOrigin: func(r *http.Request) bool { return true },
	}
	return func(w http.ResponseWriter, r *http.Request) {
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			return
		}

		client := hub.NewClient(conn)
		h.Clients[client] = true

		userDataMessage := message.UserDataMessage(isLocalConnection(r))
		err = client.SendJSON(userDataMessage)
		if err != nil {
			return
		}

		h.Mutex.RLock()
		mpMsg := message.MasterPlayerMessage(h.Configuration.Master.Volume, h.Configuration.Master.Muted)
		h.Mutex.RUnlock()
		err = client.SendJSON(mpMsg)
		if err != nil {
			return
		}

		go readLoop(h, client)
	}
}

func readLoop(h *hub.Hub, client *hub.Client) {
	defer func() {
		delete(h.Clients, client)
		err := client.Conn.Close()
		if err != nil {
			return
		}
	}()

	for {
		_, data, err := client.Conn.ReadMessage()
		if err != nil {
			return
		}

		var base message.BaseMessage
		if err := json.Unmarshal(data, &base); err != nil {
			continue
		}

		switch base.Type {
		case message.MasterPlayer:
			var msg message.Message[message.MasterPlayerPayload]
			if err := json.Unmarshal(data, &msg); err != nil {
				continue
			}
			h.Mutex.Lock()
			h.Configuration.Master.Volume = msg.Payload.Volume
			h.Configuration.Master.Muted = msg.Payload.Muted
			h.Mutex.Unlock()
		}

		h.Broadcast <- hub.Envelope{Sender: client, Data: data}
	}
}
