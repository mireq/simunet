class SNDevice:
	__stopped = False;
	__processFramePortHandlers = []
	__portHandlers = {}

	def sendFrame(self, port, data):
		if self.__stopped:
			return
		snsimulate.sendFrame(self.pSNDevice, self.deviceId, port, data)
	def sendTelnet(self, text, cmd):
		if self.__stopped:
			return
		snsimulate.sendTelnet(self.pSNDevice, self.deviceId, text, cmd)
	def processFrame(self, hwPort, data):
		try:
			self.__portHandlers[hwPort](data)
		except KeyError:
			print("port handler not defined")
	def resetConfig(self):
		print("resetConfig not implemented")
	def setConfig(self, data):
		print("setConfig not implemented")
	def dumpConfig(self):
		print("dumpConfig not implemented")
	def httpRequest(self, url, get, post):
		print("httpRequest not implemented")
	def telnetRequest(self, line, symbol):
		print("telnetRequest not implemented")
	def telnetGetControlChars(self):
		print("telnetGetControlChars not implemented")
	def stop(self, stopDev = True):
		self.__stopped = stopDev
	def insertHwPort(self):
		snsimulate.insertHwPort(self.pSNDevice)
	def removeHwPort(self, port):
		snsimulate.removeHwPort(self.pSNDevice, port)
	def setHwPortHandler(self, hwPort, func):
		self.__portHandlers[hwPort] = func
