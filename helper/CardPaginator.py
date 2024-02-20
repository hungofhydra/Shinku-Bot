from .paginator import *


class CardPaginator(Paginator):
	def __init__(self):
		
		self.showOutOfStockButton = discord.ui.Button(label="out of stock")
		self.clickedShowOutOfStockButton = False
		
		
		self.tempTotalPageCount = None
		self.tempPages = None
		self.tempCurrentPage = None
		
		super().__init__(timeout=120)
		
	
	async def start(self, ctx: discord.Interaction | commands.Context, inStock: list[discord.Embed], outOfStock: list[discord.Embed]):
		if isinstance(ctx, discord.Interaction):
			ctx = await commands.Context.from_interaction(ctx)
		
		if len(inStock) == 0:
			raise ValueError("'pages' does not contain at least one instance of discord.Embed")
		
		self.pages = inStock
		self.totalPageCount = len(inStock)
		self.ctx = ctx
		self.currentPage = self.initialPage
		
		self.previousButton.callback = self.previousButtonCallback
		self.nextButton.callback = self.nextButtonCallback
		self.pageCounter = PageCounter(style=self.pageCounterStyle,
										totalPages=self.totalPageCount,
										initialPage=self.initialPage)
		
		self.add_item(self.previousButton)
		self.add_item(self.pageCounter)
		self.add_item(self.nextButton)
		
		self.tempTotalPageCount = len(outOfStock)
		
		if self.tempTotalPageCount > 0:
			self.tempPages = outOfStock
			self.tempCurrentPage = self.initialPage
			
			self.showOutOfStockButton.callback = self.showOutOfStockButtonCallback
			self.add_item(self.showOutOfStockButton)
		
		self.message = await ctx.send(embed=self.pages[self.initialPage], view=self, ephemeral=self.ephemeral)
	
	
	async def changeEmbed(self):
		self.totalPageCount, self.tempTotalPageCount = self.tempTotalPageCount, self.totalPageCount
		self.pages, self.tempPages = self.tempPages, self.pages
		self.currentPage, self.tempCurrentPage = self.tempCurrentPage, self.currentPage
		
		self.pageCounter.label = f"{self.currentPage + 1}/{self.totalPageCount}"
		await self.message.edit(embed=self.pages[self.currentPage], view=self)
	
	
	async def showOutOfStockButtonCallback(self, interaction: discord.Interaction):
		if interaction.user != self.ctx.author:
			return await self.sendCannotControl(interaction)
		
		if not self.clickedShowOutOfStockButton:
			self.showOutOfStockButton.label = "back"
			self.clickedShowOutOfStockButton = True
		else:
			self.showOutOfStockButton.label = "out of stock"
			self.clickedShowOutOfStockButton = False
		
		await self.changeEmbed()
		await interaction.response.defer()
	
	
	