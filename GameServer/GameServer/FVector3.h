#pragma once

struct FVector3
{
	FVector3();
	FVector3(float x, float y, float z);

	FVector3 operator-(const FVector3& other) const;
	FVector3 operator+(const FVector3& other) const;
	FVector3 operator*(float scalar) const;

	float Length() const;

	FVector3 Normalize() const;
	bool IsNear(const FVector3& other, float threshold) const;

	float _x, _y, _z;
};

