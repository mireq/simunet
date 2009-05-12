from threading import Timer
from random import random
import sys
import types
import SimuNet
import re

class router(SimuNet.SNDevice):
	timerStarted = False
	timer = None
	def __init__(self):
		print("init")
		self.insertHwPort()
	def __del__(self):
		print("del")
	def __sendMsg(self):
		print("ok")
		self.sendTelnet("vystup\n", "cmd > ")
		if (self.timerStarted):
			self.timer = Timer(random(), self.__sendMsg)
			self.timer.start()
	def telnetRequest(self, line, symbol):
		p = re.compile('add')
		m = p.match(line)
		if m:
			self.insertHwPort()

		p = re.compile('rem (\d*)')
		m = p.match(line)
		if m:
			self.removeHwPort(int(m.group(1)))

		p = re.compile('send (\d*) (.*)')
		m = p.match(line)
		if m:
			self.sendFrame(int(m.group(1)), m.group(2))

		if symbol == '?':
			self.timer = Timer(random(), self.__sendMsg)
			self.timer.start()
			self.timerStarted = True
		else:
			self.timerStarted = False
		self.sendTelnet('\n', 'cmd > ');
	def stop(self):
		SimuNet.SNDevice.stop(self)
		self.timerStarted = False
		if not self.timer is None:
			self.timer.cancel()
			self.timer = None
	def processFrame(self, hwPort, data):
		print(data.__class__)
		self.sendTelnet("prijate " + data, "\n");

