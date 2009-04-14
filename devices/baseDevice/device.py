class SNDevice:
	def sendFrame(self, data):
		snsimulate.sendFrame(self.pSNDevice, self.deviceId, data)
	def sendTelnet(self, text, cmd):
		snsimulate.sendTelnet(self.pSNDevice, self.deviceId, text, cmd)
	def processFrame(self, data):
		print("processFrame not implemented")
	def resetConfig(self):
		print("resetConfig not implemented")
	def setConfig(self, data):
		print("setConfig not implemented")
	def dumpConfig(self):
		print("dumpConfig not implemented")
	def httpRequest(self, url, post):
		print("httpRequest not implemented")
	def telnetRequest(self, line, symbol):
		print("telnetRequest not implemented")
	def telnetGetControlChars(self):
		print("telnetGetControlChars not implemented")
