class testmodule(SNDevice):
	def __init__(self):
		print("init")
	def __del__(self):
		print("del " + str(self.deviceId))
	def processFrame(self, data):
		print(data)
		self.sendFrame(data[::-1]);
		return True
	def telnetRequest(self, line, symbol):
		print("telnet:" + line + ":" + symbol)
		self.sendTelnet(line[::-1], "cmd")
