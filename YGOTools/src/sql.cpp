#include "sql.h"


SqliteDb::SqliteDb(const std::string& dbName)
{
	int rc = sqlite3_open(dbName.c_str(), &m_db);
	if (rc != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(m_db));
	}
}


SqliteStmt::SqliteStmt(sqlite3* db, const std::string& stmt)
{
	int rc {sqlite3_prepare_v2(db, stmt.c_str(), -1, &m_stmt, nullptr)};
	if (rc != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(db));
	}
}


void SqliteStmt::bindValues(const std::vector<int>& types, va_list args)
{
	for (size_t i {}; i < types.size(); i++)
	{
		int type = types[i];
		switch (type)
		{
		case BIND_INT:
		{
			int n {va_arg(args, int)};
			int rc;
			if (n)
				rc = sqlite3_bind_int(m_stmt, i + 1, n);
			else
				rc = sqlite3_bind_null(m_stmt, i + 1);
			
			if (rc != SQLITE_OK)
				throw std::runtime_error(sqlite3_errstr(rc));
			
			break;
		}
		case BIND_STRING: //Must be C-String
		{
			const char* str {va_arg(args, const char*)};
			int rc {sqlite3_bind_text(m_stmt, i + 1, str, -1, SQLITE_STATIC)};
			
			if (rc != SQLITE_OK)
				throw std::runtime_error(sqlite3_errstr(rc));
			
			break;
		}
		default:
			break;
		}
	}
}


int SqliteStmt::resetSelfAndBinds()
{
	sqlite3_clear_bindings(m_stmt);
	return sqlite3_reset(m_stmt);
}


void SqliteStmt::execute()
{
	int rc = sqlite3_step(m_stmt);
	if (rc != SQLITE_DONE)
	{
		throw std::runtime_error(sqlite3_errstr(rc));
	}
}


void SqliteStmt::execute(const std::vector<int>& types, ...)
{
	va_list args;
	va_start(args, types);
	
	bindValues(types, args);
	va_end(args);
	
	execute();
}