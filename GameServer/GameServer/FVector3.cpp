#include "pch.h"
#include "FVector3.h"

FVector3 FVector3::operator-(const FVector3& other) const
{
	return FVector3{ x - other.x, y - other.y, z - other.z };
}

FVector3 FVector3::operator+(const FVector3& other) const
{
	return FVector3{ x + other.x, y + other.y, z + other.z };
}

FVector3 FVector3::operator*(float scalar) const
{
	return FVector3{ x * scalar, y * scalar, z * scalar };
}

float FVector3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

FVector3 FVector3::Normalize() const
{
	float len = Length();
	return len > 0 ? FVector3{ x / len, y / len, z / len } : FVector3{ 0, 0, 0 };
}

bool FVector3::IsNear(const FVector3& other, float threshold) const
{
	return ((*this - other).Length() < threshold);
}
