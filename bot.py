import discord
import os
from discord.ext import commands
from discord import app_commands
from dotenv import load_dotenv

load_dotenv()

class Bot(commands.Bot):
	def __init__(self):
		intents = discord.Intents.all()
		self.token = os.getenv('BOT_TOKEN')
		self.guildIdForOwnerCommand = #guild id you want
		
		
		super().__init__(command_prefix="$", intents=intents)
		
