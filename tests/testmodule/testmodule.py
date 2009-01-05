class testmodule:
	def __init__(self):
		print("init")
	def __del__(self):
		print("del " + str(self.device_id))