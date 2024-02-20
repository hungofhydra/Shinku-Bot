#ifndef CARD_SEARCHER_H
#define CARD_SEARCHER_H


#include "CardDb.h"
#include "SearchEngine.h"


class CardSearcher
{
public:
	CardSearcher(const std::string&);
	
	void loadCards();
	
	CardInfo searchById(const uint32_t&, bool);
	CardInfo searchByName(std::string&);
	
	
private:
	std::string m_db;
	VCardInfo m_cards;
	VCardInfo m_sortByPasswordCards;
	SearchEngine engine;
};



#endif