#include "pch.h"
#include "FVector3.h"

FVector3::FVector3() :_x(0), _y(0), _z(0)
{
}

FVector3::FVector3(float x, float y, float z) : _x(x), _y(y), _z(z)
{
}

FVector3 FVector3::operator-(const FVector3& other) const
{
	return FVector3{ _x - other._x, _y - other._y, _z - other._z };
}

FVector3 FVector3::operator+(const FVector3& other) const
{
	return FVector3{ _x + other._x, _y + other._y, _z + other._z };
}

FVector3 FVector3::operator*(float scalar) const
{
	return FVector3{ _x * scalar, _y * scalar, _z * scalar };
}

float FVector3::Length() const
{
	return sqrtf(_x * _x + _y * _y + _z * _z);
}

FVector3 FVector3::Normalize() const
{
	float len = Length();
	return len > 0 ? FVector3{ _x / len, _y / len, _z / len } : FVector3{ 0, 0, 0 };
}

bool FVector3::IsNear(const FVector3& other, float threshold) const
{
	return ((*this - other).Length() < threshold);
}
