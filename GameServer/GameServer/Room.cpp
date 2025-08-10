#include "pch.h"
#include "Room.h"
#include "GameSession.h"
#include "ObjectUtils.h"
#include "Player.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
}

Room::~Room()
{}

bool Room::EnterRoom(ObjectRef object, bool randPos)
{

	bool success = AddObject(object);
	if (randPos)
	{
		float x = Utils::GetRandom(0.f, 300.f);
		float y = Utils::GetRandom(0.f, 300.f);
		float z = 90.f;
		float yaw = Utils::GetRandom(0.f, 100.f);

		object->_posInfo->set_x(x);
		object->_posInfo->set_y(y);
		object->_posInfo->set_z(z);
		object->_posInfo->set_yaw(yaw);
	}

	// ���� ����� ���� �÷��̾�� �˸���
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);

		Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
		playerInfo->CopyFrom(*object->_objectInfo);
		enterGamePkt.set_allocated_player(playerInfo);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	// ���� ����� �ٸ� �÷��̾�� �˸���
	{
		Protocol::S_SPAWN spawnPkt;

		Protocol::ObjectInfo* objectInfo = spawnPkt.add_players();
		objectInfo->CopyFrom(*object->_objectInfo);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, object->_objectInfo->object_id());
	}

	// ���� ������ �÷��̾� ����� ���� �÷��̾����� �������ش�
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::S_SPAWN spawnPkt;

		for (auto& item : _objects)
		{
			if (item.second->IsPlayer() == false)
				continue;

			Protocol::ObjectInfo* playerInfo = spawnPkt.add_players();
			playerInfo->CopyFrom(*item.second->_objectInfo);
		}

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	return success;
}

bool Room::LeaveRoom(ObjectRef object)
{
	if (object == nullptr)
		return false;

	const uint64 objectId = object->_objectInfo->object_id();
	bool success = RemoveObject(objectId);

	// ���� ����� �����ϴ� �÷��̾�� �˸���
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::S_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(leaveGamePkt);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	// ���� ����� �˸���
	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_object_ids(objectId);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectId);

		if (auto player = dynamic_pointer_cast<Player>(object))
			if (auto session = player->_session.lock())
				session->Send(sendBuffer);
	}

	return success;
}

bool Room::HandleEnterPlayer(PlayerRef player)
{
	return EnterRoom(player, true);
}

bool Room::HandleLeavePlayer(PlayerRef player)
{
	return LeaveRoom(player);
}

void Room::HandleMove(Protocol::C_MOVE pkt)
{
	const uint64 objectId = pkt.info().object_id();
	if (_objects.find(objectId) == _objects.end())
		return;

	PlayerRef player = dynamic_pointer_cast<Player>(_objects[objectId]);
	player->_destinationInfo->CopyFrom(pkt.info());

	// SSG:
	if (player->IsMove()) return;

	TestTick(player);

}

void Room::HandleMoveItem(Protocol::C_MOVE_ITEM pkt)
{
	// SSG: ���� üũ ���� �ϴ� ���, �ϴ� ��ο��� ������ ����
	// TODO: Validation Check
	Protocol::S_MOVE_ITEM moveItemPkt;
	moveItemPkt.set_from_object_id(pkt.from_object_id());
	moveItemPkt.set_to_object_id(pkt.to_object_id());
	moveItemPkt.set_from_equipment_slot_type(pkt.from_equipment_slot_type());
	moveItemPkt.set_to_equipment_slot_type(pkt.to_equipment_slot_type());
	moveItemPkt.set_item_transfer_type(pkt.item_transfer_type());
	moveItemPkt.set_from_slot_pos_x(pkt.from_slot_pos_x());
	moveItemPkt.set_from_slot_pos_y(pkt.from_slot_pos_y());
	moveItemPkt.set_to_slot_pos_x(pkt.to_slot_pos_x());
	moveItemPkt.set_to_slot_pos_y(pkt.to_slot_pos_y());
	moveItemPkt.set_movable_count(pkt.movable_count());

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(moveItemPkt);
	Broadcast(sendBuffer);
}

void Room::HandleEquipItem(Protocol::C_EQUIP_ITEM pkt)
{
	// TODO: Validation Check
	Protocol::S_EQUIP_ITEM equipItemPkt;
	equipItemPkt.set_object_id(pkt.object_id());
	equipItemPkt.set_equip_state(pkt.equip_state());

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(equipItemPkt);
	Broadcast(sendBuffer);
}

void Room::TestTick(PlayerRef player)
{
	if (player == nullptr) return;
	
	player->SetMove(true);
	
	bool isTick = true;
	float DeltaTime = 1.0f / 30.0f;

	float destination_x = player->_destinationInfo->x();
	float destination_y = player->_destinationInfo->y();
	float destination_z = player->_destinationInfo->z();
	FVector3 destination = FVector3(destination_x, destination_y, destination_z);

	float location_x = player->_posInfo->x();
	float location_y = player->_posInfo->y();
	float location_z = player->_posInfo->z();
	FVector3 location = FVector3(location_x, location_y, location_z);

	FVector3 ToDest = destination - location;
	float Distance = ToDest.Length();

	FVector3 Direction = ToDest.Normalize();
	float MoveDistance = 600.f * DeltaTime;

	if (MoveDistance >= Distance) {
		player->_posInfo->CopyFrom(*player->_destinationInfo);
		isTick = false;
		player->SetMove(false);
	}
	else {
		location = location + Direction * MoveDistance;
		player->_posInfo->set_x(location._x);
		player->_posInfo->set_y(location._y);
		player->_posInfo->set_z(location._z);
	}

	Protocol::S_MOVE movePkt;
	Protocol::PosInfo* info = movePkt.mutable_info();
	info->CopyFrom(*player->_posInfo);
	
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
	Broadcast(sendBuffer);

	if(isTick) DoTimer(DeltaTime, &Room::TestTick, player);
}

RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}

bool Room::AddObject(ObjectRef object)
{
	// �ִٸ� ������ �ִ�.
	if (_objects.find(object->_objectInfo->object_id()) != _objects.end())
		return false;

	_objects.insert(make_pair(object->_objectInfo->object_id(), object));

	object->_room.store(GetRoomRef());

	return true;
}

bool Room::RemoveObject(uint64 objectId)
{
	// ���ٸ� ������ �ִ�.
	if (_objects.find(objectId) == _objects.end())
		return false;

	ObjectRef object = _objects[objectId];
	PlayerRef player = dynamic_pointer_cast<Player>(object);
	if (player)
		player->_room.store(weak_ptr<Room>());

	_objects.erase(objectId);

	return true;
}


void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : _objects)
	{
		PlayerRef player = dynamic_pointer_cast<Player>(item.second);
		if (player == nullptr)
			break;

		if (player->_objectInfo->object_id() == exceptId)
			continue;

		if (GameSessionRef session = player->_session.lock())
			session->Send(sendBuffer);
	}
}
