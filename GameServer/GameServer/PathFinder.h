#pragma once


class PathFinder
{
public:
	void ReadFile(const WCHAR* className);
	FVector3 parseToVector(const String& str);

private:
	Map<FVector3, Vector<FVector3>> EdgeMap;
};

