#ifndef DBUPDATER_H
#define DBUPDATER_H

#include "CardDb.h"
#include "simdjson.h"

#include <pybind11/embed.h>


class DBUpdater
{
public:
	DBUpdater(const std::string&);
	
	void perform();

private:
	void handleJapaneseSets(simdjson::ondemand::array&&, std::string&);
	void handleSingle(std::string_view, uint32_t&, auto&&);
	void handleSingle(std::string_view, std::string&, auto);
	
	static int busyHandler(void*, int);
	
	std::string m_db;
	pybind11::module_ pyFunc;
};

#endif
