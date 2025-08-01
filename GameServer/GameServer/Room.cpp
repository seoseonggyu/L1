#include "pch.h"
#include "Room.h"
#include "GameSession.h"
#include "ObjectUtils.h"

Room::Room()
{
}

Room::~Room()
{}


RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}



void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	/*for (auto& item : _objects)
	{
		PlayerRef player = dynamic_pointer_cast<Player>(item.second);
		if (player == nullptr)
			break;

		if (player->objectInfo->object_id() == exceptId)
			continue;

		if (GameSessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}*/
}
