#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/pytypes.h>
#include <pybind11/pybind11.h>


#include "BigwebPriceFinder.h"
#include "CardDb.h"
#include "DBUpdater.h"
#include "CardSearcher.h"
#include "format.h"


using namespace pybind11::literals;


PYBIND11_MAKE_OPAQUE(std::vector<CardInfo, std::allocator<CardInfo>>);
PYBIND11_MAKE_OPAQUE(std::vector<CardPrice, std::allocator<CardPrice>>);


VCardInfo sliceV(const VCardInfo& v, const py::slice& slice)
{
	auto first {v.begin() + slice.attr("start").cast<size_t>()};
	auto last {v.begin() + slice.attr("stop").cast<size_t>() + 1};
	VCardInfo slicedV (first, last);
	
	return slicedV;
}

PYBIND11_MODULE(YGOTools, m)
{
	py::class_<CardInfo> cardInfo (m, "CardInfo");
	
	cardInfo.def
	(
		py::init
		<
			const std::string&, const std::string&, const std::vector<std::string>&,
			const uint32_t&, const uint32_t&
		>()
	)
	
	
	.def_readwrite("enName", &CardInfo::enName)
	.def_readwrite("jaName", &CardInfo::jaName)
	.def_readwrite("jaSets", &CardInfo::jaSets)
	.def_readwrite("password", &CardInfo::password)
	.def_readwrite("konamiId", &CardInfo::konamiId)
	.def("__repr__", [](const CardInfo& c)
					{
						return "\nen name: " + c.enName + "\nja name: " + c.jaName + "\n";
					}
		);
	
	
	py::class_<VCardInfo> vCardInfo (m, "VCardInfo");
	
	vCardInfo.def(py::init<>())
		.def("clear", &VCardInfo::clear)
		.def("emplace_back", (CardInfo&(VCardInfo::*)(std::string&&, std::string&&, std::vector<std::string>&&, uint32_t&&, uint32_t&&)) &VCardInfo::emplace_back)
		.def("pop_back", &VCardInfo::pop_back)
		.def("__setitem__", [](VCardInfo& v, size_t i, const CardInfo& value) { return v[i] = value; }, py::return_value_policy::reference)
		.def("__getitem__", [](const VCardInfo& v, size_t i) { return v[i]; }, py::return_value_policy::reference)
		.def("__getitem__", &sliceV, py::return_value_policy::move)
		.def("__len__", [](const VCardInfo &v) { return v.size(); })
		.def("__iter__", [](VCardInfo &v) {
			return py::make_iterator(v.begin(), v.end());
		}, py::keep_alive<0, 1>());
	
	
	py::class_<CardPrice> cardPrice (m, "CardPrice");
	
	cardPrice.def
	(
		py::init
		<
			const std::string&, const std::string&, const std::string&,
			const std::string&, const std::string&, const int&
		>()
	)
	.def_readwrite("name", &CardPrice::name)
	.def_readwrite("printCode", &CardPrice::printCode)
	.def_readwrite("rarity", &CardPrice::rarity)
	.def_readwrite("condition", &CardPrice::condition)
	.def_readwrite("imageUrl", &CardPrice::imageUrl)
	.def_readwrite("price", &CardPrice::price)
	.def("__repr__", [](const CardPrice& c)
					{
						return c.getString();
					}
		);
	
	
	py::class_<CardPrices> cardPrices (m, "CardPrices");
	
	cardPrices.def(py::init<>())
		.def_readwrite("inStock", &CardPrices::inStock)
		.def_readwrite("outOfStock", &CardPrices::outOfStock)
		.def_readwrite("found", &CardPrices::found);
	
	
	py::class_<VCardPrice> vCardPrice (m, "VCardPrice");
	
	vCardPrice.def(py::init<>())
		.def("clear", &VCardPrice::clear)
		.def("emplace_back", (CardPrice&(VCardPrice::*)(std::string&&, std::string&&, std::string&&, std::string&&, std::string&&, int&&)) &VCardPrice::emplace_back)
		.def("pop_back", &VCardPrice::pop_back)
		.def("__setitem__", [](VCardPrice& v, size_t i, const CardPrice& value) { return v[i] = value; }, py::return_value_policy::reference)
		.def("__getitem__", [](const VCardPrice& v, size_t i) { return v[i]; }, py::return_value_policy::reference)
		.def("__len__", [](const VCardPrice &v) { return v.size(); })
		.def("__iter__", [](VCardPrice &v) {
			return py::make_iterator(v.begin(), v.end());
		}, py::keep_alive<0, 1>());
	
	
	py::class_<CardDb> cardDb (m, "CardDb");
	
	cardDb.def(py::init<const std::string&>())
		.def("queryCardInfo", py::overload_cast<const std::string&>(&CardDb::queryCardInfo));
	
	
	py::class_<DBUpdater> dbUpdater (m, "DBUpdater");
	
	dbUpdater.def(py::init<const std::string&>())
		.def("perform", &DBUpdater::perform);
	
	
	py::class_<BigwebPriceFinder> priceFinder (m, "PriceFinder");
	
	priceFinder.def(py::init<>())
		.def("findPrices", py::overload_cast<const std::string&>(&BigwebPriceFinder::findPrices))
		.def("findPrices", py::overload_cast<const CardInfo&>(&BigwebPriceFinder::findPrices));
	
	
	py::class_<CardSearcher> cardSearcher (m, "CardSearcher");
	
	cardSearcher.def(py::init<const std::string&>())
		.def("searchByName", &CardSearcher::searchByName, py::return_value_policy::move)
		.def("searchById", &CardSearcher::searchById, "id"_a, "isKonamiId"_a = true, py::return_value_policy::move)
		.def("loadCards", &CardSearcher::loadCards);
	
	
	m.def("getAuxDir", &Format::getAuxDir);
}
	