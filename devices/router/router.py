from threading import Timer
from random import random
import sys

class router(SNDevice):
	def __init__(self):
		print("init")
		t = Timer(random(), self.__sendMsg)
		#t.start()
	def __del__(self):
		print("del")
	def __sendMsg(self):
		print("a")
		sys.stdout.flush()
		pass