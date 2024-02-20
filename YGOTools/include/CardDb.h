#ifndef CARDDB_H
#define CARDDB_H

#include "sql.h"
#include "card.h"


class CardDb
{
public:
	CardDb(const CardDb&) = delete;
	CardDb& operator=(const CardDb&) = delete;
	
	CardDb(const std::string& dbName) : m_db{dbName} {}
	sqlite3* get() const { return m_db.get(); }
	VCardInfo queryCardInfo(const std::string& stmt);
	VCardInfo queryCardInfo(const std::string& stmt, const std::vector<int>& types, ...);
private:
	VCardInfo executeAndHandle(sqlite3_stmt* stmt);
	void handleCol(sqlite3_stmt* stmt, int col, CardInfo& cardInfo);
	
	SqliteDb m_db;
};



#endif