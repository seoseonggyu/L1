#pragma once

class ObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session, Protocol::CharacterClassType classType = Protocol::CHARACTER_CLASS_TYPE_NONE);

private:
	static atomic<int64> s_idGenerator;
};

