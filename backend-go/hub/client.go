package hub

import (
	"errors"
	"log"

	"github.com/gorilla/websocket"
)

type Client struct {
	Conn *websocket.Conn
}

func (c *Client) Send(data []byte) {
	if err := c.Conn.WriteMessage(websocket.TextMessage, data); err != nil {
		log.Println("Failed to send message:", err)
	}
}

func NewClient(conn *websocket.Conn) *Client {
	return &Client{Conn: conn}
}

func (c *Client) SendJSON(v interface{}) error {
	if err := c.Conn.WriteJSON(v); err != nil {
		return errors.New("failed to send message")
	}
	return nil
}
