#include "CardDb.h"
#include "format.h"
#include "pystring.h"


void CardDb::handleCol(sqlite3_stmt* stmt, int col, CardInfo& cardInfo)
{
	const std::string_view colName(sqlite3_column_name(stmt, col));
	
	if (colName == "en_name")
	{
		cardInfo.enName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
		cardInfo.adjustedEnName = cardInfo.enName;
		Format::removeAndToLower(cardInfo.adjustedEnName);
	}
	else if (colName == "ja_name")
		cardInfo.jaName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
	else if (colName == "ja_sets")
	{
		std::string_view temp {reinterpret_cast<const char*>(sqlite3_column_text(stmt, col))};
		Format::strip(temp);
		pystring::split(temp.data(), cardInfo.jaSets, " ", -1);
	}
	else if (colName == "password")
		cardInfo.password = static_cast<uint32_t>(sqlite3_column_int(stmt, col));
	else if (colName == "konami_id")
		cardInfo.konamiId = static_cast<uint32_t>(sqlite3_column_int(stmt, col));
}


VCardInfo CardDb::executeAndHandle(sqlite3_stmt* stmt)
{
	VCardInfo cardInfos;
	
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		CardInfo cardInfo;
		for (int i = 0; i < sqlite3_column_count(stmt); ++i)
		{
			handleCol(stmt, i, cardInfo);
		}
		cardInfos.emplace_back(cardInfo);
	}
	return cardInfos;
}


VCardInfo CardDb::queryCardInfo(const std::string& stmt)
{
	SqliteStmt sqlStmt(m_db.get(), stmt);
	return executeAndHandle(sqlStmt.get());
}


VCardInfo CardDb::queryCardInfo(const std::string& stmt, const std::vector<int>& types, ...)
{
	SqliteStmt sqlStmt(m_db.get(), stmt);
	va_list args;
	va_start(args, types);
	
	sqlStmt.bindValues(types, args);
	va_end(args);
	return executeAndHandle(sqlStmt.get());
}


