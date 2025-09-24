#include "pch.h"
#include "PathFinder.h"

void PathFinder::ReadFile(const WCHAR* fileName)
{
	wifstream file(fileName);

	if (!file)
	{
		wcout << L"EdgesMapOutput Error" << endl;
	}
    
    String line;
    FVector3 currentKey;

	while (getline(file, line))
	{
		line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());

		if (line.substr(0, 4) == L"Key:")
		{
			currentKey = parseToVector(line.substr(4));
			EdgeMap[currentKey];
		}
		else if (line.substr(0, 6) == L"Value:")
		{
			FVector3 value = parseToVector(line.substr(6));
			EdgeMap[currentKey].push_back(value);
		}
	}
}

FVector3 PathFinder::parseToVector(const String& str)
{
	FVector3 vec;
	swscanf_s(str.c_str(), L"(%f,%f,%f)", &vec._x, &vec._y, &vec._z);
	return vec;
}
