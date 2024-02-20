import discord
from discord.ext import commands
from discord import app_commands

class Bot(commands.Bot):
	def __init__(self):
		intents = discord.Intents.all()
		self.token = "your bot token"
		self.guildIdForOwnerCommand = #guild id you want
		
		
		super().__init__(command_prefix="$", intents=intents)
		