#include <time.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>
#include <string>

#include "core/SNSimulate.h"

using namespace std;

void printHelp(const char *progname, ostream &out)
{
	out<<"Usage:"<<endl;
	out<<progname<<" test output_file"<<endl;
}


class PerformanceTest
{
	public:
		PerformanceTest();
		virtual ~PerformanceTest();
		void start();
		double timeVal();
	protected:
		virtual void run() = 0;
	private:
		void startTimer();
		void stopTimer();

		struct timeval Tps;
		struct timeval Tpe;
};

PerformanceTest::PerformanceTest()
{
}

PerformanceTest::~PerformanceTest()
{
}

void PerformanceTest::start()
{
	startTimer();
	run();
	stopTimer();
}

void PerformanceTest::startTimer()
{
	gettimeofday(&Tps, NULL);
}

void PerformanceTest::stopTimer()
{
	gettimeofday(&Tpe, NULL);
}

double PerformanceTest::timeVal()
{
	double t1 = Tps.tv_sec + (Tps.tv_usec / 1000000.0);
	double t2 = Tpe.tv_sec + (Tpe.tv_usec / 1000000.0);
	return t2 - t1;
}

/* ------------------------------------------------------------------ */

class initTest: public PerformanceTest
{
	public:
		initTest();
	protected:
		void run();
};

initTest::initTest()
{
	start();
}

void initTest::run()
{
	SNSimulate s(1);
}

/* ------------------------------------------------------------------ */

class startDevicesTest: public PerformanceTest
{
	public:
		startDevicesTest();
		~startDevicesTest();
		void startDevices(int devices);
	protected:
		void run();
	private:
		int m_devices;
		SNSimulate *s;
};

startDevicesTest::startDevicesTest()
	:m_devices(0)
{
	s = new SNSimulate(1);
}

startDevicesTest::~startDevicesTest()
{
	delete s;
}

void startDevicesTest::startDevices(int devices)
{
	m_devices = devices;
	start();
}

void startDevicesTest::run()
{
	size_t last = 0;
	size_t first = 0;
	for (int i = 0; i < m_devices; ++i)
	{
		last = s->startDevice("generic");
		if (first == 0)
		{
			first = last;
		}
	}

	for (uint32_t i = first; i <= last; ++i)
	{
		s->stopDevice(i);
	}
}

/* ------------------------------------------------------------------ */


int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		if (argc > 0)
		{
			printHelp(argv[0], cerr);
		}
		else
		{
			printHelp("...", cerr);
		}
		return -1;
	}

	string test = argv[1];

	ofstream file;
	file.open(argv[2], ios::out);
	if (!file.is_open())
	{
		cerr << "Unable to open file " << argv[2] << endl;
	}

	if (test == "init")
	{
		initTest t;
		file<<t.timeVal()<<endl;
	}
	else if (test == "startdev")
	{
		int start = 0;
		int end = 100000;
		int step = 500;
		startDevicesTest s;
		for (int i = start; i <= end; i += step)
		{
			s.startDevices(i);
			file<<i<<", "<<s.timeVal()<<endl;
		}
	}
	else
	{
		file.close();
		printHelp(argv[0], cerr);
		return -2;
	}
	file.close();

	return 0;
}
