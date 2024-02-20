#pragma once

#include <string_view>
#include <array>


class FuzzyLogic
{
public:
	FuzzyLogic(const std::string_view&, const std::string_view&);
	
	double score;

private:
	void checkSubString();
	void calMatches();
	void calTranspositions();
	void calJaro();
	void calPrefix();
	void calScore();
	

	int m, n;
	std::string_view s1, s2;
	double p {0.1};
	std::array<bool, 100> match1{};
	std::array<bool, 100> match2{};
	int matches {};
	int transpositions {};
	int prefix {};
	int suffix {};
	int subStringBonus {};
	double jaro;
};