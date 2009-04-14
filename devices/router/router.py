from threading import Timer
from random import random
import sys
import types
import SimuNet

class router(SimuNet.SNDevice):
	timerStarted = False
	def __init__(self):
		print("init")
	def __del__(self):
		print("del")
	def __sendMsg(self):
		print("ok")
		self.sendTelnet("vystup\n", "cmd > ")
		if (self.timerStarted):
			t = Timer(random(), self.__sendMsg)
			t.start()
	def telnetRequest(self, line, symbol):
		if symbol == '?':
			t = Timer(random(), self.__sendMsg)
			t.start()
			self.timerStarted = True
		else:
			self.timerStarted = False
			return "\nkoncim\ncmd > "
