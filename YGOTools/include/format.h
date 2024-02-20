#ifndef FORMAT_H
#define FORMAT_H

#include <fstream>
#include <string>
#include <array>
#include <unordered_map>


#include "json.hpp"


namespace Format
{
	inline constexpr std::array<char, 2> removeChars {'?', '!'};
	inline const std::array<std::wstring, 11> patterns {L"Ｄ―", L"ＢＦ―", L"ＨＥＲＯ ", L"ＴＧ ", L"ＴＧ―", L"ＲＵＭ―", L"Ｕ．Ａ．", L"Ｓ－Ｆｏｒｃｅ", L"ＲＲ―", L"ＧＰ－", L"Ｐ．Ｕ．Ｎ．Ｋ．"};
	inline const std::unordered_map<const char*, int> patternsForRegex {{".*Ｎｏ．[０-９]{6}.*", 5}, {".*Ｎｏ．[０-９]{9}.*", 6}};


	void halfToFull(std::string&);
	void strip(std::string_view&);
	void strip(std::string&);
	void replace(std::string&, const std::string&, const std::string&);
	std::string sliceStringContainsUTF16(const std::string&, int, int);
	void formatByConfig(std::string& str, const nlohmann::json&);
	void formatByConfig(std::string_view& str, const nlohmann::json&);
	void formatRarity(std::string_view&);
	void formatJaName(std::string&);
	void removeAndToLower(std::string&);
	std::string strViewToStr(const std::string_view);
	std::string getAuxDir(const std::string&);
	
	inline const nlohmann::json rarityAlias {nlohmann::json::parse(std::ifstream(Format::getAuxDir("map/rarityAlias.json")))};
	inline const nlohmann::json nameAlias {nlohmann::json::parse(std::ifstream(Format::getAuxDir("map/nameAlias.json")))};
}


#endif