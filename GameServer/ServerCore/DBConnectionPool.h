#pragma once
#include "DBConnection.h"

/*-------------------
	DBConnectionPool
--------------------*/

class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool					Connect(int32 connectionCount, const WCHAR* connectionString);
	void					Clear();

	DBConnection*			Pop();
	void					Push(DBConnection* connection);

private:
	USE_LOCK;
	SQLHENV					_environment = SQL_NULL_HANDLE;
	Vector<DBConnection*>	_connections;
};

class DBConnectionPoolGuard
{
public:
    DBConnectionPoolGuard(DBConnectionPool& pool)
        : _pool(&pool), _conn(pool.Pop()) {}

    ~DBConnectionPoolGuard() {
        if (_pool && _conn) _pool->Push(_conn);
    }

    DBConnection* operator->() const { return _conn; }
    DBConnection& operator*()  const { return *_conn; }
    explicit operator bool()   const { return _conn != nullptr; }

    DBConnection* release() { auto t = _conn; _conn = nullptr; return t; }

private:
    DBConnectionPool* _pool;
    DBConnection* _conn;
};