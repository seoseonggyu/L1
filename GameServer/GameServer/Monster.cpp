#include "pch.h"
#include "Monster.h"
#include "PathFinder.h"

Monster::Monster()
{
	_objectInfo->set_object_type(Protocol::OBJECT_TYPE_MONSTER);
}

Monster::~Monster()
{
}

void Monster::TestAStar()
{
	FVector3 start = FVector3(_posInfo->x(), _posInfo->y(), 0.f);
	FVector3 end = FVector3(1000, -200, 0);
	GPathFinder->AStar(start, end, _path);
}

void Monster::Update()
{
    if (_pathIdx >= _path.size())
        return;

    FVector3 target = _path[_pathIdx];
    FVector3 dir{ target._x - _posInfo->x(), target._y - _posInfo->y(), target._z - _posInfo->z() };

    float dist = sqrt(dir._x * dir._x + dir._y * dir._y + dir._z * dir._z);

    if (dist < 1e-3f)
    {
        _pathIdx++;
        return;
    }

    dir._x /= dist;
    dir._y /= dist;
    dir._z /= dist;

    float moveDist = 200 * 0.1f;

    if (moveDist >= dist)
    {
        _posInfo->set_x(target._x);
        _posInfo->set_y(target._y);
        _posInfo->set_z(target._z);
        _pathIdx++;
    }
    else
    {
        float x = _posInfo->x();
        float y = _posInfo->y();
        float z = _posInfo->z();
        x += dir._x * moveDist;
        y += dir._y * moveDist;
        z += dir._z * moveDist;

        _posInfo->set_x(x);
        _posInfo->set_y(y);
        _posInfo->set_z(z);
    }

    cout << "NowPos(" << _posInfo->x() << ", " << _posInfo->y() << ", " << _posInfo->z() << ")" << endl;
}
