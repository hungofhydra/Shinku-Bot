#include <cmath>
#include <cstring>
#include <algorithm>


#include "SearchEngine.h"
#include "format.h"
#include "fuzzy.h"
#include "pystring.h"


SearchEngine::SearchEngine(const VCards& cards)
{
	load(cards);
}


void SearchEngine::load(const VCards& cards)
{
	loadCards(cards);
	createInvertedIndex();
}


void SearchEngine::createInvertedIndex()
{
	InvertedIndex index;
	for (const auto& card : m_cards)
	{
		VStrings words;
		pystring::split(card.adjustedEnName, words, " ", -1);
		
		for (const auto& word : words)
		{
			if (!index.contains(word))
			{
				index.emplace(word, VCardsPointer());
			}
			
			index[word].emplace_back(std::make_shared<CardInfo>(card));
		}
	}
	m_index = index;
}


void SearchEngine::loadCards(const VCards& cards)
{
	m_cards = cards;
	m_vCardsSize = m_cards.size();
}


VCardsPPD SearchEngine::search(std::string& query)
{
	Format::removeAndToLower(query);
	
	TFIDF q;
	VStrings qWords;
	pystring::split(query, qWords, " ", -1);
	
	for (const auto& word : qWords)
	{
		if (m_index.find(word) != m_index.end())
		{
			q[word] = calculateTFIDF(word, qWords);
		}
	}
	
	VCardsPPD result;
	for (const auto& card : m_cards)
	{
		if (strcmp(query.c_str(), card.adjustedEnName.c_str()) == 0)
		{
			result.emplace_back(std::make_shared<CardInfo>(card), 100.0);
			break;
		}
		
		VStrings cWords;
		pystring::split(card.adjustedEnName, cWords, " ", -1);
		TFIDF c {createTFIDFVector(cWords)};
		
		double cosine {calculateCosineSimilarity(q, c, query, card.adjustedEnName)};
		double jaroForEach {calculateAverageJaroWinkler(qWords, cWords)};
		FuzzyLogic jaro {query, card.adjustedEnName};
		
		
		result.emplace_back(std::make_shared<CardInfo>(card), calculateFinalScore(cosine, jaro.score, jaroForEach));
	}
	
	std::sort(result.begin(), result.end(), [](const auto& a, const auto& b)
											{
												return a.second > b.second;
											});
	return result;
}


double SearchEngine::calculateTFIDF(const std::string& word, const VStrings& words)
{
	double tf {};
	
	
	for (const auto& w : words)
	{
		if (w == word) { ++tf; }
	}
	
	tf /= words.size();
	double idf {std::log(m_vCardsSize / m_index.at(word).size())};
	
	return tf * idf;
}


TFIDF SearchEngine::createTFIDFVector(const VStrings& words)
{
	TFIDF vec;
	
	for (const auto& word : words)
	{
		vec[word] = calculateTFIDF(word, words);
	}
	return vec;
}


double SearchEngine::calculateCosineSimilarity(const TFIDF& q, const TFIDF& c, const std::string& query, const std::string& cardName)
{
	double dotProduct {};
	for (const auto& [word, weight] : q)
	{
		if (c.find(word) != c.end())
		{
			dotProduct += weight * c.at(word);
		}
	}
	
	
	double qLength {};
	double cLength {};
	for (const auto& [word, weight] : q)
	{
		qLength += weight * weight;
	}
	qLength = std::sqrt(qLength);
	
	for (const auto& [word, weight] : c)
	{
		cLength += weight * weight;
	}
	cLength = std::sqrt(cLength);
	
	
	double cosine {dotProduct / (qLength * cLength)};
	
	return cosine;
}


double SearchEngine::calculateAverageJaroWinkler(const VStrings& qWords, const VStrings& cWords)
{
	double similarity {0.0};
	
	for (const auto& qw : qWords)
	{
		double maxJW {0.0};
		for (const auto& cw : cWords)
		{
			FuzzyLogic jw {qw, cw};
			if (jw.score > maxJW) { maxJW = jw.score; }
		}
		
		similarity += maxJW;
	}
	
	similarity /= qWords.size();
	
	return similarity;
}


double SearchEngine::calculateFinalScore(const double& cosine, const double& jaro, const double& averageJaroWinkler)
{
	if (cosine == cosine)
	{
		return averageJaroWinkler + cosine * (0.5 * jaro);
	}
	
	return averageJaroWinkler;
}


