
class SimulateHelper:
	def __init__(self):
		import Queue
		import thread
		self.__jobs = Queue.Queue()
		thread.start_new_thread(self.run, ())
		print "Starting thread."

	def put(self, targetDevice, job):
		if job is None:
			self.__jobs.put(None)
		else:
			self.__jobs.put([targetDevice, job])

	def run(self):
		while True:
			job = self.__jobs.get()
			if job is None:
				print "Stopping thread."
				return
			else:
				print "Job for device " + str(job[0])
				try:
					snsimulate.devices[job[0]].processFrame(job[1])
				except KeyError:
					print "Device " + str(job[0]) + " not found."
