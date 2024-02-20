#include "format.h"
#include <uchar.h>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/uniset.h>
#include <unicode/utypes.h>
#include <unicode/utrans.h>
#include <unicode/translit.h>
#include <utility>
#include <cctype>
#include <algorithm>
#include <locale>
#include <regex>


namespace Format
{
	void halfToFull(std::string& str)
	{
		UErrorCode errorCodeTrans {U_ZERO_ERROR};
		icu::Transliterator* trans {icu::Transliterator::createInstance("Halfwidth-Fullwidth", UTRANS_FORWARD, errorCodeTrans)};

		icu::UnicodeString uStr {icu::UnicodeString::fromUTF8(str)};
		icu::UnicodeSet filter("[^[:space:]~]", errorCodeTrans);

		trans->adoptFilter(&filter);
		trans->transliterate(uStr);

		str = "";
		uStr.toUTF8String(str);
	}


	void strip(std::string_view& str)
	{
		auto begin {std::find_if_not(str.begin(), str.end(), [](const auto& c) {
			return std::isspace(c);
		})};

		auto end {std::find_if_not(str.rbegin(), str.rend(), [](const auto& c) {
			return std::isspace(c);
		}).base()};

		str = std::string_view(begin, end - begin);
	}


	void strip(std::string& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));

		str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), str.end());
	}


	void replace(std::string& subject, const std::string& search, const std::string& replace)
	{
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos)
		{
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}


	std::string sliceStringContainsUTF16(const std::string& str, int start, int end)
	{
		int size = str.length();

		start = std::clamp(start, -static_cast<int>(size), static_cast<int>(size));
		end = std::clamp(end, -static_cast<int>(size), static_cast<int>(size));

		if (start < 0) { start += size; }
		if (end < 0) { end += size; }
		if (start >= end) { return ""; }

		std::wstring wStr {std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.from_bytes(str.c_str(), str.c_str() + size)};

		for (const auto& pattern : Format::patterns)
		{
			size_t pos {wStr.find(pattern)};
			if (pos != std::string::npos)
			{
				start = pos + pattern.length();
				end = start + 4;
			}
		}


		wStr = wStr.substr(start, end - start);

		return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(wStr);
	}


	void formatByConfig(std::string_view& str, const nlohmann::json& aliasConfig)
	{
		if (aliasConfig[0].contains(str))
			str = aliasConfig[0][str].get<std::string_view>();
	}


	void formatByConfig(std::string& str, const nlohmann::json& aliasConfig)
	{
		if (aliasConfig[0].contains(str))
			str = aliasConfig[0][str].get<std::string>();
	}


	void formatRarity(std::string_view& rarity)
	{
		Format::strip(rarity);
		Format::formatByConfig(rarity, Format::rarityAlias);
	}


	void formatJaName(std::string& name)
	{
		Format::formatByConfig(name, Format::nameAlias);
		Format::replace(name, "－", "―");
		Format::halfToFull(name);
		Format::replace(name, "琰魔竜", "魔竜");

		if (name.starts_with("ブラック")) { return; }
		else if (name.starts_with("ナンバ")) { return; }

		int end {4};
		int start {};


		for (const auto& [pattern, value] : Format::patternsForRegex)
		{
			std::regex re {pattern};
			if (std::regex_match(name, re))
			{
				end = value;
				if (name.starts_with("Ｆ") || name.starts_with("Ｃ") || name.starts_with("Ｓ"))
					++end;

				break;
			}
		}

		name = Format::sliceStringContainsUTF16(name, start, end);
		Format::strip(name);
	}

	void removeAndToLower(std::string& name)
	{
		for (auto& c : Format::removeChars)
		{
			name.erase(std::remove(name.begin(), name.end(), c), name.end());
		}
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (name.starts_with("number")) { name.erase(std::remove(name.begin(), name.end(), ':'), name.end()); }
	}


	std::string strViewToStr(const std::string_view view)
	{
		return std::string(view.begin(), view.size());
	}
	
	
	std::string getAuxDir(const std::string& destinationPoint)
	{
		if (const char* auxDir = std::getenv("AUX_DIR"))
		{
			return std::string(auxDir) + "/" + destinationPoint;
		}
		
		return "aux/" + destinationPoint;
	}
}