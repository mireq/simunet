#include "core/SNDevice.h"
#include <Python.h>

int main(int argc, char *argv[])
{
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
	PyRun_SimpleString("sys.path.append(os.getcwd())");
	PyRun_SimpleString("devices = {}");
	for (int i = 1; i < argc; ++i)
	{
		SNDevice test(argv[i], i);
	}
	Py_Finalize();
}
