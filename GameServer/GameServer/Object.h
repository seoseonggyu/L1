#pragma once

class Object : public enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();

public:


public:
	virtual float GetDamage(Protocol::SkillType skillType);
	Protocol::CharacterClassType GetClassType() { return _objectInfo->character_classtype(); }
	bool IsPlayer() { return _isPlayer; }

public:
	Protocol::ObjectInfo*	_objectInfo;
	Protocol::PosInfo*		_posInfo;
	Protocol::PosInfo*		_destinationInfo;
	Protocol::VitalInfo*	_vitalInfo;
	Protocol::StatInfo*		_statInfo;

public:
	atomic<weak_ptr<Room>> _room;

protected:
	bool _isPlayer = false;
};