#pragma once

struct FVector3
{
	FVector3();
	FVector3(float x, float y, float z);

	FVector3 operator-(const FVector3& other) const;
	FVector3 operator+(const FVector3& other) const;
	FVector3 operator*(float scalar) const;

	bool operator<(const FVector3& other) const;
	bool operator==(const FVector3& other) const;

	float Length() const;

	FVector3 Normalize() const;
	bool IsNear(const FVector3& other, float threshold) const;

	float _x, _y, _z;
};

struct FVector3Hash
{
	std::size_t operator()(const FVector3& v) const noexcept
	{
		std::size_t h1 = std::hash<float>{}(v._x);
		std::size_t h2 = std::hash<float>{}(v._y);
		std::size_t h3 = std::hash<float>{}(v._z);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
};