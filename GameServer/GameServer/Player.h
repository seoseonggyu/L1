#pragma once
#include "Object.h"

class GameSession;

class Player : public Object
{
public:
	Player();
	virtual ~Player();

public:
	float GetDamage(Protocol::SkillType skillType) override;

public:
	weak_ptr<GameSession> _session;
	
private:
	// SSG: 임시로 데미지 설정
	float _damage = 10.f;
};