#ifndef SQL_H
#define SQL_H

#define BIND_INT 1
#define BIND_STRING 2

#include <stdexcept>
#include <vector>
#include <string>
#include "sqlite3.h"

class SqliteDb
{
public:
	SqliteDb(const SqliteDb&) = delete;
	SqliteDb& operator=(const SqliteDb&) = delete;
	
	SqliteDb(const std::string& dbName);
	~SqliteDb()
	{
		sqlite3_close(m_db);
	}
	sqlite3* get() const { return m_db; }

private:
	sqlite3* m_db {nullptr};
};


class SqliteStmt
{
public:
	SqliteStmt(const SqliteStmt&) = delete;
	SqliteStmt& operator=(const SqliteStmt&) = delete;
	
	SqliteStmt(sqlite3* db, const std::string& stmt);
	~SqliteStmt() { sqlite3_finalize(m_stmt); }
	sqlite3_stmt* get() const { return m_stmt; }
	void bindValues(const std::vector<int>&, va_list);
	int resetSelfAndBinds();
	void execute();
	void execute(const std::vector<int>&, ...);

private:
	sqlite3_stmt* m_stmt {nullptr};
};

#endif