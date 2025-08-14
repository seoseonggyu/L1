#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	bool EnterRoom(ObjectRef object, bool randPos = true);
	bool LeaveRoom(ObjectRef object);

	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleSelcetClass(Protocol::C_SELECTCLASS pkt);
	void HandleMove(Protocol::C_MOVE pkt);
	void HandleMoveItem(Protocol::C_MOVE_ITEM pkt);
	void HandleEquipItem(Protocol::C_EQUIP_ITEM pkt);
	void HandleSkillImmediateCast(Protocol::C_SKILL_IMMEDIATE_CAST pkt);


public:
	// SSG:
	void TestTick(PlayerRef player);
	
	RoomRef GetRoomRef();

private:
	bool AddObject(ObjectRef object);
	bool RemoveObject(uint64 objectId);

private:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:
	unordered_map<uint64, ObjectRef> _objects;
};

extern RoomRef GRoom;