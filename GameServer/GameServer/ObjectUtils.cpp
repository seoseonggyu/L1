#include "pch.h"
#include "ObjectUtils.h"
#include "ObjectPool.h"
#include "GameSession.h"
#include "Player.h"
#include "Monster.h"

atomic<int64> ObjectUtils::s_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session, Protocol::CharacterClassType classType)
{
	const int64 newId = s_idGenerator.fetch_add(1);

	PlayerRef player = ObjectPool<Player>::MakeShared();
	player->_objectInfo->set_object_id(newId);
	player->_objectInfo->set_character_classtype(classType);
	player->_posInfo->set_object_id(newId);
	player->_vitalInfo->set_hp(1.f);
	player->_vitalInfo->set_max_hp(1.f);

	

	player->_session = session;
	session->_player.store(player);

	return player;
}

MonsterRef ObjectUtils::CreateMonster(Protocol::MonsterType monsterType)
{
	const int64 newId = s_idGenerator.fetch_add(1);

	MonsterRef monster = ObjectPool<Monster>::MakeShared();
	monster->_objectInfo->set_object_id(newId);
	monster->_objectInfo->set_monster_type(monsterType);
	monster->_posInfo->set_object_id(newId);

	// SSG: DB에서 값을 꺼내서 세팅해야함
	monster->_vitalInfo->set_hp(50);
	monster->_vitalInfo->set_max_hp(50);

	return monster;
}