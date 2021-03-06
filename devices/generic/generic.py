# -*- coding: UTF-8 -*-
from threading import Timer
from random import random
import sys
import types
import SimuNet
import re

class generic(SimuNet.SNDevice):
	def __init__(self):
		print("init")
		self.prikazy = []
		self.timerStarted = False
		self.timer = None
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
		self.prikazy.append(line)

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
	def httpRequest(self, url, get, post):
		print("url:" + url);
		print(get);
		print(post);

		stranka = ""
		prikazySel = ""
		hlStrSel = " class=\"selected\""
		if url == "/prikazy":
			stranka = "prikazy"
			prikazySel = " class=\"selected\""
			hlStrSel = ""

		nazov = "Hlavná stránka"
		if stranka == "prikazy":
			nazov = "Posledné príkazy"

		string = """\
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
	<title>"""
		string = string + nazov
		string = string + """</title>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</head>
<div>
<h1>"""
		string = string + nazov
		string = string + """</h1>
<div id="page">
	<div id="left">"""
		string = string + "<ul id=\"menu\">"
		string = string + "<li"+hlStrSel+">"
		string = string + "<a href=\"simunet:/?dev_id="+str(self.deviceId)+"\">Hlavná stránka</a>"
		string = string + "  <ul>";
		string = string + "  <li"+prikazySel+">";
		string = string + "  <a href=\"simunet:/prikazy\">Posledné príkazy</a>"
		string = string + "  </ul>";
		string = string + "</li>"
		string = string + "</ul>"
		string = string + """
	</div>
	<div id="main" class="main">"""
		if stranka == "prikazy":
			string = string + """
		<table>
			<tr>
				<th>&nbsp;</th><th>Príkaz</th>
			</tr>
"""
			for i in range(len(self.prikazy)):
				string = string + "<tr><td>"+str(i)+"</td><td>"+self.prikazy[i]+"</td></tr>"
			string = string + """
		</table>"""
		string = string + """
		</div>
	</div>
</body>
</html>
"""
		return string

