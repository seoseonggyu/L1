#include "pch.h"
#include "ObjectUtils.h"
#include "GameSession.h"
#include "Player.h"

atomic<int64> ObjectUtils::s_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session, Protocol::CharacterClassType classType)
{
	const int64 newId = s_idGenerator.fetch_add(1);

	PlayerRef player = make_shared<Player>();
	player->_objectInfo->set_object_id(newId);
	player->_objectInfo->set_character_classtype(classType);
	player->_posInfo->set_object_id(newId);

	player->_session = session;
	session->_player.store(player);

	return player;
}
