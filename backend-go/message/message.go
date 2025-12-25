package message

type MessageType string

type BaseMessage struct {
	Type MessageType `json:"type"`
}

type Message[T any] struct {
	Type    MessageType `json:"type"`
	Payload T           `json:"payload"`
}

const (
	UserData     MessageType = "USER_DATA"
	MasterPlayer MessageType = "MASTER_PLAYER"
)

type UserDataPayload struct {
	IsLocal bool `json:"is_local"`
}

type MasterPlayerPayload struct {
	Volume int32 `json:"volume"`
	Muted  bool  `json:"muted"`
}

func MasterPlayerMessage(volume int32, muted bool) Message[MasterPlayerPayload] {
	return Message[MasterPlayerPayload]{
		Type: MasterPlayer,
		Payload: MasterPlayerPayload{
			Volume: volume,
			Muted:  muted,
		},
	}
}

func UserDataMessage(isLocal bool) Message[UserDataPayload] {
	return Message[UserDataPayload]{
		Type: UserData,
		Payload: UserDataPayload{
			IsLocal: isLocal,
		},
	}
}
