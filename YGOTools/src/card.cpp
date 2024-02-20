#include <sstream>


#include "card.h"


std::string CardPrice::getString() const
{
	std::ostringstream oss;
	oss << "Name: " << name << "\n";
	oss << "Print code: " << printCode << "\n";
	oss << "Rarity: " << rarity << "\n";
	oss << "Condition: " << condition << "\n";
	oss << "Image URL: " << imageUrl << "\n";
	oss << "Price: " << price << "\n";
	return oss.str();
}


std::ostream& operator<<(std::ostream& out, const CardPrice& card)
{
	out << "name: " << card.name << '\n'
	<< "print code: " << card.printCode << '\n'
	<< "rarity: " << card.rarity << '\n'
	<< "condition: " << card.condition << '\n'
	<< "price: " << card.price << '\n'
	<< "image url: " << card.imageUrl << '\n';
		
	return out;
}


std::ostream& operator<<(std::ostream& out, const CardInfo& cardInfo)
{
	out << "en name: " << (cardInfo.enName != "" ? cardInfo.enName : "NONE") << '\n'
	<< "ja name: " << (cardInfo.jaName != "" ? cardInfo.jaName : "NONE") << '\n'
	<< "ja sets:";
	if (cardInfo.jaSets.size() > 0)
	{
		for (const auto& s : cardInfo.jaSets)
		{
			out << ' ' << s;
		}
	}
	else { out << "NONE"; }
	out << '\n';
	out << "password: " << cardInfo.password << '\n'
	<< "konami id: " << cardInfo.konamiId;
		
	return out;
}