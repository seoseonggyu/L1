#pragma once

struct FVector3
{
	float x, y, z;

	FVector3 operator-(const FVector3& other) const;
	FVector3 operator+(const FVector3& other) const;
	FVector3 operator*(float scalar) const;

	float Length() const;

	FVector3 Normalize() const;
	bool IsNear(const FVector3& other, float threshold) const;
};

