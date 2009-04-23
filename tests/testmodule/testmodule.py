import SimuNet

class testmodule(SimuNet.SNDevice):
	__config = "";

	def __init__(self):
		print("init")
	def __del__(self):
		print("del " + str(self.deviceId))
	def processFrame(self, data):
		print(data)
		self.sendFrame(data[::-1]);
		return False
	def telnetRequest(self, line, symbol):
		print("telnet:" + line + ":" + symbol)
		self.sendTelnet(line[::-1], "cmd")
	def resetConfig(self):
		print("reset")
		return False
	def setConfig(self, data):
		print("cfg:" + data)
		self.__config = data
		return False
	def dumpConfig(self):
		return self.__config
	def httpRequest(self, url, post):
		postName = post.keys()[0];
		print "hr:"+url+":"+postName+":"+post[postName];
		return "htmlrecv"
	def telnetGetControlChars(self):
		return "abc"
