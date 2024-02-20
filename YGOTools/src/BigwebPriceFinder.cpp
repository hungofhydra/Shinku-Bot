#include "BigwebPriceFinder.h"
#include "format.h"

const std::string BIGWEB_API_ENDPOINT = "https://api.bigweb.co.jp/products?game_id=9&name=";


void BigwebPriceFinder::process(CardPrice& card, auto&& rawPrice)
{
	rawPrice["name"].get_string().get(card.name);
	rawPrice["rarity"]["slip"].get_string().get(card.rarity);
	rawPrice["condition"]["slip"].get_string().get(card.condition);
	rawPrice["image"].get_string().get(card.imageUrl);
	card.price = rawPrice["price"].get_int64();
	
	
	Format::formatRarity(card.rarity);
	card.condition = ((card.condition == "特価[傷含む]") ? "Scratch" : "Good");
}


void BigwebPriceFinder::insert(CardPrices& cardPrices, CardPrice& card)
{
	if (card.price > 0)
		cardPrices.inStock.emplace_back(card);
	else
		cardPrices.outOfStock.emplace_back(card);
}

	
void BigwebPriceFinder::handleRawPrices(CardPrices& cardPrices, ondemand::array rawPrices, const CardInfo& cardInfo = CardInfo())
{
	if (rawPrices.is_empty())
	{
		cardPrices.found = false;
		return;
	}
	
	
	cardPrices.found = true;
	CardPrice card;
	
	
	for (auto&& rawPrice : rawPrices)
	{
		card.printCode = Format::strViewToStr(rawPrice["fname"].get_string());
		Format::strip(card.printCode);
		
		
		if (cardInfo.jaSets.size() > 0)
		{
			if (std::find(cardInfo.jaSets.begin(), cardInfo.jaSets.end(), card.printCode) == cardInfo.jaSets.end() || card.printCode == "")
				continue;
		}
		
		
		process(card, rawPrice);
		insert(cardPrices,  card);
	}
	
	
	if (cardPrices.inStock.size() > 0)
		std::sort(cardPrices.inStock.begin(), cardPrices.inStock.end(), [](const CardPrice& a, const CardPrice& b) -> bool { return a.price < b.price; });
}


ondemand::array BigwebPriceFinder::getRawPrices(const std::string& name)
{
	ondemand::document* doc {m_getPrices.jsonFromUrl(BIGWEB_API_ENDPOINT + name + "&sortBy=id")};
	if (doc)
	{
		return (*doc)["items"].get_array();
	}
	
	return ondemand::array();
}

	
CardPrices BigwebPriceFinder::findPrices(const std::string& name)
{
	CardPrices cardPrices {};
	handleRawPrices(cardPrices, getRawPrices(name));
	
	return cardPrices;
}


CardPrices BigwebPriceFinder::findPrices(const CardInfo& card)
{
	CardPrices cardPrices {};
	handleRawPrices(cardPrices, getRawPrices(card.jaName), card);
	
	return cardPrices;
}