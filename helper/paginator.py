import discord
from discord.ext import commands


class Paginator(discord.ui.View):
	def __init__(self, *,
				timeout: int = 60,
				previousButton: discord.ui.Button = discord.ui.Button(emoji=discord.PartialEmoji(name="\U000025c0")),
				nextButton: discord.ui.Button = discord.ui.Button(emoji=discord.PartialEmoji(name="\U000025b6")),
				pageCounterStyle: discord.ButtonStyle = discord.ButtonStyle.grey,
				initialPage: int = 0,
				allowOthers: bool = False,
				deleteOnTimeout: bool = False,
				ephemeral: bool = False) -> None:
		
		
		self.previousButton = previousButton
		self.nextButton = nextButton
		self.pageCounterStyle = pageCounterStyle
		self.initialPage = initialPage
		self.allowOthers = allowOthers
		self.deleteOnTimeout = deleteOnTimeout
		self.ephemeral = ephemeral
		
		self.pages = None
		self.ctx = None
		self.message = None
		self.currentPage = None
		self.pageCounter = None
		self.totalPageCount = None
		
		super().__init__(timeout=timeout)
	
	
	async def start(self, ctx: discord.Interaction | commands.Context, pages: list[discord.Embed]):
		if isinstance(ctx, discord.Interaction):
			ctx = await commands.Context.from_interaction(ctx)
		
		if len(pages) == 0:
			raise ValueError("'pages' does not contain at least one instance of discord.Embed")
		
		self.pages = pages
		self.totalPageCount = len(pages)
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
		
		self.message = await ctx.send(embed=self.pages[self.initialPage], view=self, ephemeral=self.ephemeral)
	
	
	async def previous(self):
		if self.currentPage == 0:
			self.currentPage = self.totalPageCount - 1
		else:
			self.currentPage -= 1
		
		self.pageCounter.label = f"{self.currentPage + 1}/{self.totalPageCount}"
		await self.message.edit(embed=self.pages[self.currentPage], view=self)
	
	
	async def next(self):
		if self.currentPage == self.totalPageCount - 1:
			self.currentPage = 0
		else:
			self.currentPage += 1
		
		self.pageCounter.label = f"{self.currentPage + 1}/{self.totalPageCount}"
		await self.message.edit(embed=self.pages[self.currentPage], view=self)
	
	
	async def sendCannotControl(self, interaction: discord.Interaction):
		embed = discord.Embed(description="You cannot control this pagination!",
								color=discord.Colour.red())
		
		return await interaction.response.send_message(embed=embed, ephemeral=True)
	
	
	async def nextButtonCallback(self, interaction: discord.Interaction):
		if interaction.user != self.ctx.author and not self.allowOthers:
			return await self.sendCannotControl(interaction)
		
		await self.next()
		await interaction.response.defer()
	
	
	async def previousButtonCallback(self, interaction: discord.Interaction):
		if interaction.user != self.ctx.author and not self.allowOthers:
			return await self.sendCannotControl(interaction)
		
		await self.previous()
		await interaction.response.defer()
	
	
	async def on_timeout(self) -> None:
		if self.deleteOnTimeout:
			await self.message.delete()
	
	
class PageCounter(discord.ui.Button):
	def __init__(self, style: discord.ButtonStyle, totalPages, initialPage):
		super().__init__(label=f"{initialPage + 1}/{totalPages}", style=style, disabled=True)