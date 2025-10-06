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
			_EdgeMap[currentKey];
		}
		else if (line.substr(0, 6) == L"Value:")
		{
			FVector3 value = parseToVector(line.substr(6));
			_EdgeMap[currentKey].push_back(value);
		}
	}
}

FVector3 PathFinder::parseToVector(const String& str)
{
	FVector3 vec;
	swscanf_s(str.c_str(), L"(%f,%f,%f)", &vec._x, &vec._y, &vec._z);
	return vec;
}

void PathFinder::AStar(FVector3 start, FVector3 dest, Vector<FVector3>& outPath)
{
	FVector3 front[] =
	{
		FVector3 { -1, 0, 0},
		FVector3 { 0, -1, 0},
		FVector3 { 1, 0, 0},
		FVector3 { 0, 1, 0},
		FVector3 {-1, -1, 0},
		FVector3 {1, -1, 0},
		FVector3 {1, 1, 0},
		FVector3 {-1, 1, 0},
	};

	int32 cost[] =
	{
		10,
		10,
		10,
		10,
		14,
		14,
		14,
		14
	};

	Map<FVector3, FVector3> parent;
	HashSet<FVector3, FVector3Hash> closed;

	priority_queue<PQNode, Vector<PQNode>, greater<PQNode>> pq;
	int32 g = 0;
	int32 h = static_cast<int32>(10 * (abs(dest._y - start._y) + abs(dest._x - start._x)));
	pq.push(PQNode{ g + h, g, start });

	parent[start] = start;

	while (pq.empty() == false)
	{
		PQNode node = pq.top();
		pq.pop();

		if (closed.find(node.pos) != closed.end()) continue;
		closed.insert(node.pos);

		if (node.pos == dest)
			break;

		for (int32 dir = 0; dir < DIR_COUNT; dir++)
		{
			FVector3 nextPos = node.pos + (front[dir] * 100.f);
			
			auto it = _EdgeMap.find(node.pos);
			if (it != _EdgeMap.end()) {
				const Vector<FVector3>& values = it->second;
				auto findIt = find(values.begin(), values.end(), nextPos);
				if (findIt == values.end()) continue;
			}
			if (closed.find(nextPos) != closed.end()) continue;

			int32 g = node.g + cost[dir];
			int32 h = static_cast<int32>(10 * (abs(dest._y - nextPos._y) + abs(dest._x - nextPos._x)));
			pq.push(PQNode{ g + h, g, nextPos });
			parent[nextPos] = node.pos;
		}
	}

	FVector3 pos = dest;

	outPath.clear();
	while (true)
	{
		outPath.push_back(pos);

		if (pos == parent[pos])
			break;

		pos = parent[pos];
	}
	reverse(outPath.begin(), outPath.end());


	for (int32 i = 0; i < outPath.size(); ++i)
	{
		cout << "(" << outPath[i]._x << ", "<< outPath[i]._y << ", " << outPath[i]._z  << ") ->";
	}
}