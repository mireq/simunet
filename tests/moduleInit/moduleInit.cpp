#include "core/SNDevice.h"
#include "core/PyCPPObject.h"
#include "shared.h"
#include <Python.h>

using namespace std;

int main(int argc, char *argv[])
{
	Py_Initialize();
	PyRun_SimpleString("import gc");
	PyRun_SimpleString("gc.set_debug(gc.DEBUG_LEAK)");
	PyObject *pDevicesDict = interpreterInit();
	for (int i = 1; i < argc; ++i)
	{
		SNDevice test(argv[i], i);
		PyCPPObject frame1(PyString_FromString("frame1"));
		test.processFrame(frame1);
		PyCPPObject frame2(PyString_FromString("frame2"));
		test.processFrame(frame2);
	}
//	Py_XDECREF(pDevicesDict);
// 	PyRun_SimpleString("gc.collect()");
// 	PyRun_SimpleString("print gc.garbage");
// 	PyRun_SimpleString("print gc.get_threshold()");
	Py_Finalize();
}
