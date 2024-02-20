#ifndef REQUESTS_H
#define REQUESTS_H


#include "simdjson.h"

#include <pybind11/embed.h>
#include <string_view>
#include <string>
#include <iostream>


using namespace simdjson;
namespace py = pybind11;

class Requests
{
public:
	ondemand::document* jsonFromUrl(const std::string_view url)
	{
		try
		{
			py::object re {rGet(url)};
			std::cerr << url << '\n';
			m_json = simdjson::padded_string(re.attr("text").cast<std::string_view>());
			m_doc = parser_.iterate(m_json);
			return &m_doc;
		}
		catch (const simdjson_error& e)
		{
			std::cout << e.error() << '\n';
		}
		return nullptr;
	}
	
	ondemand::parser parser_;

private:
	simdjson::padded_string m_json;
	ondemand::document m_doc;
	py::object Requests {py::module_::import("requests")};
	py::object rGet {Requests.attr("get")};
};


#endif