from threading import Timer
from random import random
import sys
import types
import SimuNet

class router(SimuNet.SNDevice):
	timerStarted = False
	timer = None
	def __init__(self):
		print("init")
	def __del__(self):
		print("del")
	def __sendMsg(self):
		print("ok")
		self.sendTelnet("vystup\n", "cmd > ")
		if (self.timerStarted):
			self.timer = Timer(random(), self.__sendMsg)
			self.timer.start()
	def telnetRequest(self, line, symbol):
		if symbol == '?':
			self.timer = Timer(random(), self.__sendMsg)
			self.timer.start()
			self.timerStarted = True
		else:
			self.timerStarted = False
			return "\nkoncim\ncmd > "
	def stop(self):
		SimuNet.SNDevice.stop(self)
		self.timerStarted = False
		if not self.timer is None:
			self.timer.cancel()
			self.timer = None
