#include "pch.h"
#include "Object.h"

Object::Object()
{
	_objectInfo			= new Protocol::ObjectInfo();
	_posInfo			= new Protocol::PosInfo();
	_destinationInfo	= new Protocol::PosInfo();
	_vitalInfo			= new Protocol::VitalInfo();
	_statInfo			= new Protocol::StatInfo();
	
	_objectInfo->set_allocated_pos_info(_posInfo);
	_objectInfo->set_allocated_vital_info(_vitalInfo);
	_objectInfo->set_allocated_stat_info(_statInfo);

	_objectInfo->set_object_type(Protocol::OBJECT_TYPE_NONE);
	_objectInfo->set_character_classtype(Protocol::CHARACTER_CLASS_TYPE_NONE);
	_objectInfo->set_monster_type(Protocol::MONSTER_TYPE_NONE);
}

Object::~Object()
{
	delete _objectInfo;
	delete _destinationInfo;
}

float Object::GetDamage(Protocol::SkillType skillType)
{
	return 0.0f;
}