#include <cmath>
#include <algorithm>
#include <iostream>


#include <fuzzy.h>


FuzzyLogic::FuzzyLogic(const std::string_view& s1_, const std::string_view& s2_)
	: s1 {s1_}, s2 {s2_}
{
	m = s1.length();
	n = s2.length();
		
	calMatches();
	if (matches > 0)
	{
		calTranspositions();
		calJaro();
		calPrefix();
		calScore();
	}
}


void FuzzyLogic::checkSubString()
{
	size_t found {s2.find(s1)};
	if (found != std::string_view::npos) { subStringBonus = s1.length(); }
}


void FuzzyLogic::calMatches()
{
	int maxDist = std::floor(std::max(m, n) / 2) - 1;
	
	for (int i {}; i < m; ++i)
	{
		int start = std::max(0, i - maxDist);
		int end = std::min(n - 1, i + maxDist);
		
		if (s2.find_first_of(s1[i], start) == std::string::npos) { break; }
		
		for (int j {start}; j <= end; ++j)
		{
			if (match2[j]) { continue; }
			
			if (s1[i] == s2[j])
			{
				++matches;
				match1[i] = true;
				match2[j] = true;
				break;
			}
		}
	}
}


void FuzzyLogic::calTranspositions()
{
	int k {};
	for (int i {}; i < m; ++i)
	{
		if (match1[i])
		{
			while (!match2[k]) { ++k; }
			if (s1[i] != s2[k]) { ++transpositions; }
			++k;
		}
	}
}


void FuzzyLogic::calJaro()
{
	jaro = (matches / (double) m + matches / (double) n + (matches - transpositions / 2.0) / (double) matches) / 3.0;
}


void FuzzyLogic::calPrefix()
{
	for (int i {}; i < m; ++i)
	{
		if (s1.compare(i, 1, s2, i, 1) == 0) { ++prefix; }
		else { break; }
	}
}


void FuzzyLogic::calScore()
{
	int l = std::min(4, prefix);
	score = jaro + l * p * (1 - jaro);
}