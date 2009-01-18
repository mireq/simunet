
class testmodule(SNDevice):
	def __init__(self):
		print("init")
	def __del__(self):
		print("del " + str(self.__device_id))
	def processFrame(self, data):
		print(data)
		return True
