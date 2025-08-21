#include "pch.h"
#include "Player.h"

Player::Player()
{
	_isPlayer = true;
}

Player::~Player()
{
}

float Player::GetDamage(Protocol::SkillType skillType)
{
	// TODO: 파일로 깔끔하게, 싱글톤으로 스킬, 버프 매니저 생성해서 관리하자,
	
	// 1. 직업 알아야함
	Protocol::CharacterClassType classType = _objectInfo->character_classtype();

	// 2. 직업의 스킬에 맞는 딜을 추출하면 댐 + 버프나 이런 게 존재하는지 확인하고
	if (classType == Protocol::CHARACTER_CLASS_TYPE_BARBARIAN && skillType == Protocol::SkillType_Skill_1)
	{
		return _combatInfo->damage();
	}

	return 0.0f;
}
