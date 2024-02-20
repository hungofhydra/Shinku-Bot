import json

class Jsoner:
	def load_json(self, file):
		with open(file, "r") as f:
			return json.load(f)
	
	
	def json_dump(self, file, dict):
		with open(file, "w") as f:
			json.dump(dict, f, indent=4)