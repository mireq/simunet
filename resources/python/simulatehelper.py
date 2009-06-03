
class SimulateHelper:
	def __init__(self):
		import Queue
		from threading import Thread
		self.__jobs = Queue.Queue()
		self.thread = Thread(None, self.run ,None,())
		self.thread.start()
		#thread.start_new_thread(self.run, ())
		print "Starting thread."

	def put(self, targetDevice, port, job):
		if job is None:
			self.__jobs.put(None)
			self.thread.join()
		else:
			self.__jobs.put([targetDevice, port, job])

	def run(self):
		while True:
			job = self.__jobs.get()
			if job is None:
				print "Stopping thread."
				return
			else:
				print "Job for device " + str(job[0])
				try:
					snsimulate.devices[job[0]].processFrame(job[1], job[2])
				except KeyError:
					print "Device " + str(job[0]) + " not found."
