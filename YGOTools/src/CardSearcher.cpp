#include <algorithm>
#include <fstream>


#include "CardSearcher.h"
#include "fuzzy.h"



CardSearcher::CardSearcher(const std::string& dbName)
	: m_db {dbName}
{
	loadCards();
}


void CardSearcher::loadCards()
{
	CardDb db {m_db};
	m_cards = db.queryCardInfo("SELECT * FROM cards");
	m_sortByPasswordCards = m_cards;
	engine.load(m_cards);
	
	
	std::sort(m_cards.begin(), m_cards.end(), [](const CardInfo& a, const CardInfo& b)
												{
													return a.konamiId < b.konamiId;
												}
			);
	
	std::sort(m_sortByPasswordCards.begin(), m_sortByPasswordCards.end(), [](const CardInfo& a, const CardInfo& b)
												{
													return a.password < b.password;
												}
			);
	
}


CardInfo CardSearcher::searchById(const uint32_t& id, bool isKonamiId = true)
{
	VCardInfo::iterator card;
	if (isKonamiId)
	{
		card = std::lower_bound(m_cards.begin(), m_cards.end(), id, [](const CardInfo& a, const uint32_t& b) { return a.konamiId < b; });
	}
	else
	{
		card = std::lower_bound(m_sortByPasswordCards.begin(), m_sortByPasswordCards.end(), id, [](const CardInfo& a, const uint32_t& b) { return a.password < b; });
	}
	if (card != m_cards.end()) { return *card; }
	
	return {};
}


CardInfo CardSearcher::searchByName(std::string& query)
{
	return *(engine.search(query)[0].first);
}