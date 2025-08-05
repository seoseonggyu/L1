#pragma once

class Object : public enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();

	bool IsPlayer() { return _isPlayer; }
	bool IsMove() { return _isMove; }

	void SetMove(bool isMove) { _isMove = isMove; }

public:
	Protocol::ObjectInfo*	_objectInfo;
	Protocol::PosInfo*		_posInfo;
	Protocol::PosInfo*		_destinationInfo;

public:
	atomic<weak_ptr<Room>> _room;

protected:
	bool _isPlayer = false;
	
	// SSG: 
	bool _isMove = false;
};

