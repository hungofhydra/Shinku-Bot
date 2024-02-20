#ifndef BIGWEBPRICEFINDER_H
#define BIGWEBPRICEFINDER_H

#include "requests.h"
#include "card.h"



class BigwebPriceFinder
{
public:
	BigwebPriceFinder() = default;
	BigwebPriceFinder(const BigwebPriceFinder&) = delete;
	BigwebPriceFinder& operator=(const BigwebPriceFinder&) = delete;
	
	CardPrices findPrices(const std::string&);
	CardPrices findPrices(const CardInfo&);
	
private:
	ondemand::array getRawPrices(const std::string&);
	void process(CardPrice&, auto&&);
	void insert(CardPrices&, CardPrice&);
	void handleRawPrices(CardPrices&, ondemand::array, const CardInfo&);

	Requests m_getPrices;
};

#endif