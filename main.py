from helper.CardPriceFinder import CardPriceFinder
from helper.jsoner import Jsoner
from helper.path import DB_PATH, NAME_ALIAS_PATH
from YGOTools import DBUpdater
from bot import *

import sqlite3, asyncio


#setup
bot = Bot()
hybrid = bot.hybrid_command
finder = CardPriceFinder(DB_PATH)
updater = DBUpdater(DB_PATH)
jsoner = Jsoner()


@bot.event
async def on_ready():
	print("Bot is ready.")
	await bot.change_presence(activity=discord.Game(name="/help"))
	try:
		synced = await bot.tree.sync()
		syncedGuild = await bot.tree.sync(guild=discord.Object(id=bot.guildIdForOwnerCommand))
		print(f"Synced {len(synced)} command(s)")
		print(f"Synced {len(syncedGuild)} guild command(s)")
	except Exception as e:
		print(e)


@bot.event
async def on_message(message):
	if message.author.bot:
		return
	if bot.user in message.mentions:
		await message.channel.send(f"Total latency: **{round(bot.latency * 1000)} ms**")
	await bot.process_commands(message)


@hybrid(name="help", description="Send information about the bot's command")
async def help(ctx):
	embed = discord.Embed(
	title="Bot Commands", color=0xbe2ed6)
	embed.add_field(name="$cp <en name>", value="search for the OCG prices of a card by its EN name")
	embed.add_field(name="$pp", value="search for the OCG prices of a print code")
	embed.add_field(name="Slash Commands", value="enter **/search_prices** to see their information and usage")
	await ctx.send(embed=embed)


@bot.command()
async def cp(ctx, *, input: str):
	inStock, outOfStock, output, found = await finder.getPricesEmbedsByEnNameOrId(input)
	await finder.sendPricesEmbeds(ctx, inStock=inStock, outOfStock=outOfStock, name=output, found=found)


@bot.command()
async def pp(ctx, *, printCode: str):
	inStock, outOfStock, output, found = await finder.getPricesEmbedsByPrintCode(printCode)
	await finder.sendPricesEmbeds(ctx, inStock=inStock, outOfStock=outOfStock, name=output, found=found)


@hybrid(name="search_prices_by_en_name", description="search for the OCG prices of a card by its EN name")
@app_commands.describe(
    name="for example: Sky Striker Ace - Raye"
)
async def searchByEnName(ctx, *, name: str):
	await ctx.defer()
	await cp(ctx, input=name)


@hybrid(name="search_prices_by_print_code", description="search for the OCG prices of a print code")
@app_commands.describe(
    printcode="for example: DBDS-JP029"
)
async def searchByPrintCode(ctx, *, printcode: str):
	await ctx.defer()
	await pp(ctx, printCode=printcode)


@hybrid(name="search_prices_by_konami_id", description="search for the OCG prices of a card by its konami id")
@app_commands.describe(
    id="for example: 13670"
)
async def searchByKonamiId(ctx, id: int):
	await ctx.defer()
	
	inStock, outOfStock, output, found = await finder.getPricesEmbedsByEnNameOrId(id, isId = True)
	await finder.sendPricesEmbeds(ctx, inStock=inStock, outOfStock=outOfStock, name=output, found=found)


@hybrid(name="search_prices_by_password", description="search for the OCG prices of a card by its password")
@app_commands.describe(
    password="for example: 26077387"
)
async def searchByPassword(ctx, password: int):
	await ctx.defer()
	
	inStock, outOfStock, output, found = await finder.getPricesEmbedsByEnNameOrId(password, isId = True, isKonamidId = False)
	await finder.sendPricesEmbeds(ctx, inStock=inStock, outOfStock=outOfStock, name=output, found=found)


@hybrid(name = "change_alias")
@app_commands.guilds(discord.Object(id=bot.guildIdForOwnerCommand))
@commands.is_owner()
async def change_alias(ctx, old_name: str, new_name:str):
	await ctx.defer()
	names_config = jsoner.load_json(NAME_ALIAS_PATH)
	names_config[old_name] = new_name
	jsoner.json_dump(NAME_ALIAS_PATH, names_config)
	with sqlite3.connect(DB_PATH) as conn:
		conn.cursor().execute("UPDATE cards SET ja_name = ? WHERE ja_name = ?", (new_name, old_name))
		conn.commit()
	finder.searcher.loadCards()
	await ctx.send("success")


@bot.command()
@commands.is_owner()
async def update(ctx):
	loop = asyncio.get_event_loop()
	await loop.run_in_executor(None, updater.perform)
	await loop.run_in_executor(None, finder.searcher.loadCards)
	await ctx.send("Update complete")


@bot.command()
@commands.is_owner()
async def close(ctx):
	await bot.close()


@bot.command()
@commands.is_owner()
async def exit(ctx):
	exit()


bot.run(bot.token)