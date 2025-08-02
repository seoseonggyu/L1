#pragma once
#include "Object.h"

class GameSession;

class Player : public Object
{
public:
	Player();
	virtual ~Player();

public:
	weak_ptr<GameSession> _session;
};

