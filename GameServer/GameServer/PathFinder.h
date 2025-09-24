#pragma once


struct PQNode
{
	bool operator<(const PQNode& other) const { return f < other.f; }
	bool operator>(const PQNode& other) const { return f > other.f; }

	int32		f;
	int32		g;
	FVector3	pos;
};

class PathFinder
{
public:
	void ReadFile(const WCHAR* className);
	FVector3 parseToVector(const String& str);

	void AStar(FVector3 start, FVector3 dest, Vector<FVector3>& outPath);

private:
	Map<FVector3, Vector<FVector3>> _EdgeMap;
};