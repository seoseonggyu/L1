#include "pch.h"
#include "Object.h"

Object::Object()
{
	_objectInfo			= new Protocol::ObjectInfo();
	_posInfo			= new Protocol::PosInfo();
	_destinationInfo	= new Protocol::PosInfo();
	_vitalInfo			= new Protocol::VitalInfo();
	
	_objectInfo->set_allocated_pos_info(_posInfo);
	_objectInfo->set_allocated_vital_info(_vitalInfo);
}

Object::~Object()
{
	delete _objectInfo;
	delete _destinationInfo;
}
