#include "pch.h"
#include "Monster.h"

Monster::Monster()
{
	_objectInfo->set_object_type(Protocol::OBJECT_TYPE_MONSTER);
}

Monster::~Monster()
{
}
