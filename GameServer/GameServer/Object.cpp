#include "pch.h"
#include "Object.h"

Object::Object()
{
	_objectInfo = new Protocol::ObjectInfo();
	_posInfo = new Protocol::PosInfo();
	_objectInfo->set_allocated_pos_info(_posInfo);

}

Object::~Object()
{
	delete _objectInfo;
}
