#pragma once

#include "Object.h"

class Monster : public Object
{
public:
	Monster();
	virtual ~Monster();

	void TestAStar();

	virtual void Update() override;

private:
	Vector<FVector3> _path;
	int32 _pathIdx = 0;

};