from /YGOTools import PriceFinder, CardSearcher, CardInfo
from discord import Embed, Color
from .CardPaginator import CardPaginator


class CardPriceFinder:
	EXCHANGE_RATE = 180
	MAX_FIELDS = 6
	
	def __init__(self, dbName):
		self.priceFinder = PriceFinder()
		self.searcher = CardSearcher(dbName)
	
	
	def yenToVND(self, yen) -> str:
		vnd = int(yen) * self.EXCHANGE_RATE
		return format(vnd, ",") + " VND"
	
	
	def getValue(self, cardPrice) -> str:
		return f"{cardPrice.price} YEN\n{self.yenToVND(cardPrice.price)}\nCondition: {cardPrice.condition}\n[Image]({cardPrice.imageUrl})"
	
	
	def getName(self, cardPrice) -> str:
		return f"{cardPrice.printCode} ({cardPrice.rarity})"
	
	
	def createBaseEmbed(self, title: str, imageUrl: str = None):
		embed = Embed(title=title, color=Color.purple())
		if imageUrl is not None:
			embed.set_thumbnail(url=imageUrl)
		
		return embed
	
	
	def createEmbeds(self, title: str, cardPriceList: list[CardInfo], imageUrl: str = None) -> list[Embed]:
		embeds = []
		embed = self.createBaseEmbed(title=title, imageUrl=imageUrl)
		
		for i, cardPrice in enumerate(cardPriceList):
			embed.add_field(name=self.getName(cardPrice), value=self.getValue(cardPrice), inline=True)
			
			if (i + 1) % self.MAX_FIELDS == 0:
				embeds.append(embed)
				embed = self.createBaseEmbed(title=title, imageUrl=imageUrl)
		
		if len(embed.fields) > 0:
			embeds.append(embed)
		
		return embeds
	
	
	async def getPricesEmbedsByEnNameOrId(self, name, isId: bool = False, isKonamidId: bool = True) -> (list[Embed], list[Embed], str, bool):
		if isId:
			card = self.searcher.searchById(name, isKonamidId)
		else:
			card = self.searcher.searchByName(name)
		
		cardPrices = self.priceFinder.findPrices(card)
		
		inStock = self.createEmbeds(card.enName, cardPrices.inStock, imageUrl=f"https://images.ygoprodeck.com/images/cards_cropped/{card.password}.jpg")
		outOfStock = self.createEmbeds(card.enName, cardPrices.outOfStock, imageUrl=f"https://images.ygoprodeck.com/images/cards_cropped/{card.password}.jpg")
		
		return inStock, outOfStock, card.enName, cardPrices.found
	
	
	async def getPricesEmbedsByPrintCode(self, printCode: str) -> (list[Embed], list[Embed], str, bool):
		printCode = printCode.upper()
		cardPrices = self.priceFinder.findPrices(printCode)
		
		inStock = self.createEmbeds(printCode, cardPrices.inStock)
		outOfStock = self.createEmbeds(printCode, cardPrices.outOfStock)
		
		return inStock, outOfStock, printCode, cardPrices.found
	
	
	async def sendPricesEmbeds(self, ctx, inStock: list[Embed], outOfStock: list[Embed], name: str, found: bool):
		if (len(inStock) == 0):
			if found:
				await ctx.send(f"**{name}** is out of stock")
			else:
				await ctx.send(f"Sorry, currently I can’t find any prices for **{name}**.")
		else:
			await CardPaginator().start(ctx, inStock=inStock, outOfStock=outOfStock)
