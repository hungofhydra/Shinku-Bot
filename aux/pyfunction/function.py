from bs4 import BeautifulSoup

def parseAndExpandRuby(html: str) -> str:
	rubyLess = ""
	soup = BeautifulSoup(html, "html.parser")
	
	for element in soup.children:
		if element.name is None:
			rubyLess += element.string
		elif element.name == "ruby" and len(element.contents) == 2:
			rb, rt = element.contents
			if rb.name is None and rt.name == "rt" and len(rt.contents) == 1 and rt.contents[0].name is None:
				rubyLess += rb.string
	
	return rubyLess