#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	RoomRef GetRoomRef();

private:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);
};

