#ifndef CARD_H
#define CARD_H

#include <string_view>
#include <string>
#include <iostream>
#include <memory>
#include <vector>


struct CardPrice
{
	std::string_view name;
	std::string printCode;
	std::string_view rarity;
	std::string_view condition;
	std::string_view imageUrl;
	int price;
	
	std::string getString() const;
	
	friend std::ostream& operator<<(std::ostream& out, const CardPrice& card);
};


struct CardInfo
{
	std::string enName {};
	std::string jaName {};
	std::vector<std::string> jaSets {};
	uint32_t password {};
	uint32_t konamiId {};
	
	std::string adjustedEnName;
	
	friend std::ostream& operator<<(std::ostream& out, const CardInfo& cardInfo);
	
};

using VCardInfo = std::vector<CardInfo, std::allocator<CardInfo>>;
using VCardPrice = std::vector<CardPrice, std::allocator<CardPrice>>;


struct CardPrices
{
	VCardPrice inStock;
	VCardPrice outOfStock;
	bool found;
};



#endif