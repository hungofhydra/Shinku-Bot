import discord
from discord.ext import commands
from discord import app_commands

class Bot(commands.Bot):
	def __init__(self):
		intents = discord.Intents.all()
		self.token = "MTIwOTUyNDU1MDY4NTk1ODE1NQ.GePm7L.J3fL-oXiGDi6VdPSbmm4N840FpZZ3ppy1-8TPY"
		self.guildIdForOwnerCommand = #guild id you want
		
		
		super().__init__(command_prefix="$", intents=intents)
		
