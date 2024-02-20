#include <thread>
#include <chrono>


#include "DBUpdater.h"
#include "format.h"
#include "requests.h"


const std::string YAMLYUGI_ENDPOINT = "https://raw.githubusercontent.com/DawnbrandBots/yaml-yugi/aggregate/cards.json";


DBUpdater::DBUpdater(const std::string& dbName)
	: m_db {dbName}
{
	py::module_ sys {py::module_::import("sys")};
	sys.attr("path").attr("append")(Format::getAuxDir("pyfunction"));
	pyFunc = py::module_::import("function");
}


void DBUpdater::perform()
{
	CardDb db {m_db};
	if (!db.get()) { return; }
	
	sqlite3_exec(db.get(), "PRAGMA journal_mode = WAL", nullptr, nullptr, nullptr);
	sqlite3_exec(db.get(), "PRAGMA synchronous = NORMAL", nullptr, nullptr, nullptr);
	sqlite3_busy_handler(db.get(), busyHandler, nullptr);
	
	Requests request;
	ondemand::document* doc {request.jsonFromUrl(YAMLYUGI_ENDPOINT)};
	if (!doc) { return; }
	
	
    SqliteStmt stmt {db.get(), "INSERT OR REPLACE INTO cards (en_name, ja_name, ja_sets, password, konami_id) VALUES (?, ?, ?, ?, ?)"};
	sqlite3_exec(db.get(), "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
	
	
	std::string enName;
	std::string jaName;
	std::string jaSets;
	
	
	for (auto&& card : *doc)
	{
		if (card["konami_id"].is_null()) { continue; }
		
		
		uint32_t konamiId = card["konami_id"].get_uint64();
		uint32_t password {};
		
		
		try
		{
			handleJapaneseSets(card["sets"]["ja"].get_array(), jaSets);
		}
		catch (const simdjson::simdjson_error &e)
		{
			continue;
		}
		
		
		handleSingle("en", enName, card);
		jaName = pyFunc.attr("parseAndExpandRuby")(std::string_view(card["name"]["ja"])).cast<std::string>();
		Format::formatJaName(jaName);
		handleSingle("password", password, card);
		
		try
		{
			stmt.execute({BIND_STRING, BIND_STRING, BIND_STRING, BIND_INT, BIND_INT}, enName.c_str(), jaName.c_str(), jaSets.c_str(), password, konamiId);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		sqlite3_reset(stmt.get());
		
		jaSets = "";
	}
	sqlite3_exec(db.get(), "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
}


void DBUpdater::handleJapaneseSets(ondemand::array&& cardSets, std::string& result)
{
	bool space {false};
	
	for (auto first {cardSets.begin()}; first != cardSets.end(); ++first)
	{
		std::string_view setNumber {(*first)["set_number"]};
		result.append(std::string(setNumber.begin(), setNumber.size()));
		
		
		space = true;
		if (space) { result += " "; }
	}
}


void DBUpdater::handleSingle(std::string_view field, uint32_t& result, auto&& card)
{
	if (!card[field].is_null())
	{
		result = card[field].get_uint64();
	}
}


void DBUpdater::handleSingle(std::string_view field, std::string& result, auto card)
{
	if (!card["name"][field].is_null())
	{
		std::string_view name {card["name"][field]};
		result = std::string(name.begin(), name.size());
	}
}


int DBUpdater::busyHandler(void*, int count)
{
	if (count <= 10)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return 1;
	}
	return 0;
}