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
	// TODO: ���Ϸ� ����ϰ�, �̱������� ��ų, ���� �Ŵ��� �����ؼ� ��������,
	
	// 1. ���� �˾ƾ���
	Protocol::CharacterClassType classType = _objectInfo->character_classtype();

	// 2. ������ ��ų�� �´� ���� �����ϸ� �� + ������ �̷� �� �����ϴ��� Ȯ���ϰ�
	if (classType == Protocol::CHARACTER_CLASS_TYPE_BARBARIAN && skillType == Protocol::SkillType_Skill_1)
	{
		return _combatInfo->damage();
	}

	return 0.0f;
}
