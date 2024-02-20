#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H


#include <map>
#include <memory>


#include "card.h"


using SPCardInfo = std::shared_ptr<CardInfo>;
using VCards = VCardInfo;
using VCardsPointer = std::vector<SPCardInfo>;
using VCardsPPD = std::vector<std::pair<SPCardInfo, double>>;
using VStrings = std::vector<std::string>;
using InvertedIndex = std::map<std::string, VCardsPointer>;
using TFIDF = std::map<std::string, double>;


class SearchEngine
{
public:
	SearchEngine() = default;
	SearchEngine(const VCards&);
	
	void load(const VCards&);
	VCardsPPD search(std::string&);
	
private:
	void loadCards(const VCards&);
	void createInvertedIndex();
	
	double calculateTFIDF(const std::string&, const VStrings&);
	TFIDF createTFIDFVector(const VStrings&);
	
	double calculateCosineSimilarity(const TFIDF&, const TFIDF&, const std::string&, const std::string&);
	double calculateAverageJaroWinkler(const VStrings&, const VStrings&);
	double calculateFinalScore(const double&, const double&, const double&);
	
	
	InvertedIndex m_index;
	VCards m_cards;
	int m_vCardsSize;

};



#endif