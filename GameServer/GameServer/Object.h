#pragma once

class Object : public enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();

	bool IsPlayer() { return _isPlayer; }

public:
	Protocol::ObjectInfo*	_objectInfo;
	Protocol::PosInfo*		_posInfo;

	// SSG:
	FVector3 _location;
	FVector3 _destination;

public:
	atomic<weak_ptr<Room>> _room;


protected:
	bool _isPlayer = false;
};

