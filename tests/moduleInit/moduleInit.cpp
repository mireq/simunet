#include "core/PyCPPObject.h"
#include "core/SNDevice.h"
#include "core/SNExceptions.h"
#include "config.h"
#include <iostream>
#include <Python.h>

using namespace std;

int main(int argc, char *argv[])
{
/*	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
	PyRun_SimpleString("sys.path.append(os.getcwd())");
	PyRun_SimpleString("devices = {}");
	PyObject *pSysModuleName, *pOsModuleName;
	PyObject *pOsModule, *pSysModule;
	PyObject *pPathObject, *pAppendFunc;
	PyObject *pGetCwdFunc, *pCwd;
	PyObject *pMainName, *pMainModule, *pDevicesDict;
	PyObject *pGetCwdParams, *pAppendParams;
	PyObject *pDevicesLocation;*/

	Py_Initialize();
	PyCPPObject pSysModuleName(PyString_FromString("sys"));
	PyCPPObject pSysModule(PyImport_Import(pSysModuleName));
	PyCPPObject pOsModuleName(PyString_FromString("os"));
	PyCPPObject pOsModule(PyImport_Import(pOsModuleName));
	PyCPPObject pPathObject(PyObject_GetAttrString(pSysModule, "path"));
	PyCPPObject pAppendFunc(PyObject_GetAttrString(pPathObject, "append"));
	if (!pAppendFunc.isCallable())
	{
		throw new SNPythonInterpreterException("append", SNPythonInterpreterException::CALL);
	}
	PyCPPObject pGetCwdFunc(PyObject_GetAttrString(pOsModule, "getcwd"));
	if (!pGetCwdFunc.isCallable())
	{
		throw new SNPythonInterpreterException("getcwd", SNPythonInterpreterException::CALL);
	}
	PyCPPObject pGetCwdParams(PyTuple_New(0));
	PyCPPObject pCwd(PyObject_Call(pGetCwdFunc, pGetCwdParams, NULL));
	PyCPPObject pAppendParams(PyTuple_New(1));
	if (PyTuple_SetItem(pAppendParams, 0, pCwd))
	{
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::SET);
	}
	PyObject_Call(pAppendFunc, pAppendParams, NULL);
	PyCPPObject pDevicesLocation(PyString_FromString(DEVICES_LOCATION));
	if (PyTuple_SetItem(pAppendParams, 0, pDevicesLocation))
	{
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::SET);
	}
	PyObject_Call(pAppendFunc, pAppendParams, NULL);
	// natiahneme globalnu premennu devices
	PyCPPObject pMainName(PyString_FromString("__main__"));
	PyCPPObject pMainModule(PyImport_Import(pMainName));
	PyCPPObject pDevicesDict(PyDict_New());
	if (PyObject_SetAttrString(pMainModule, "devices", pDevicesDict))
	{
		throw SNPythonInterpreterException("devices", SNPythonInterpreterException::SET);
	}
	pDevicesDict.keepRef();
	for (int i = 1; i < argc; ++i)
	{
		SNDevice test(argv[i], i);
	}
/*
	pSysModuleName = PyString_FromString("sys");
	if (pSysModuleName == NULL)
	{
		throw SNPythonInterpreterException("sys string", SNPythonInterpreterException::CREATE);
	}
	pSysModule = PyImport_Import(pSysModuleName);
	if (pSysModule == NULL)
	{
		Py_XDECREF(pSysModuleName);
		throw SNPythonInterpreterException("sys", SNPythonInterpreterException::IMPORT);
	}
	Py_XDECREF(pSysModuleName);
	pOsModuleName = PyString_FromString("os");
	if (pOsModuleName == NULL)
	{
		Py_XDECREF(pSysModule);
		throw SNPythonInterpreterException("os string", SNPythonInterpreterException::CREATE);
	}
	pOsModule = PyImport_Import(pOsModuleName);
	Py_XDECREF(pOsModuleName);
	if (pOsModule == NULL)
	{
		Py_XDECREF(pSysModule);
		throw SNPythonInterpreterException("os", SNPythonInterpreterException::IMPORT);
	}

	pPathObject = PyObject_GetAttrString(pSysModule, "path");
	if (pPathObject == NULL)
	{
		Py_XDECREF(pSysModule);
		throw SNPythonInterpreterException("sys.path", SNPythonInterpreterException::ATTR);
	}
	Py_XDECREF(pSysModule);
	pAppendFunc = PyObject_GetAttrString(pPathObject, "append");
	if (pAppendFunc == NULL)
	{
		throw SNPythonInterpreterException("sys.path.append", SNPythonInterpreterException::ATTR);
	}
	if (!PyCallable_Check(pAppendFunc))
	{
		Py_XDECREF(pAppendFunc);
		throw SNPythonInterpreterException("sys.path.append", SNPythonInterpreterException::CALL);
	}
	pGetCwdFunc = PyObject_GetAttrString(pOsModule, "getcwd");
	if (pGetCwdFunc == NULL)
	{
		Py_XDECREF(pAppendFunc);
		Py_XDECREF(pOsModule);
		throw SNPythonInterpreterException("os.getcwd", SNPythonInterpreterException::ATTR);
	}
	if (!PyCallable_Check(pGetCwdFunc))
	{
		Py_XDECREF(pGetCwdFunc);
		throw SNPythonInterpreterException("os.getcwd", SNPythonInterpreterException::CALL);
	}
	Py_XDECREF(pOsModule);
	pGetCwdParams = PyTuple_New(0);
	if (pGetCwdParams == NULL)
	{
		Py_XDECREF(pAppendFunc);
		Py_XDECREF(pGetCwdFunc);
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::CREATE);
	}
	pCwd = PyObject_Call(pGetCwdFunc, pGetCwdParams, NULL);
	if (pCwd == NULL)
	{
		Py_XDECREF(pGetCwdFunc);
		throw SNPythonInterpreterException("os.getcwd", SNPythonInterpreterException::CALL);
	}
	Py_XDECREF(pGetCwdFunc);

	pAppendParams = PyTuple_New(1);
	if (pAppendParams == NULL)
	{
		Py_XDECREF(pAppendFunc);
		Py_XDECREF(pCwd);
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::CREATE);
	}

	if (PyTuple_SetItem(pAppendParams, 0, pCwd))
	{
		Py_XDECREF(pAppendFunc);
		Py_XDECREF(pCwd);
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::SET);
	}
	PyObject_Call(pAppendFunc, pAppendParams, NULL);
	Py_XDECREF(pCwd);

	pDevicesLocation = PyString_FromString(DEVICES_LOCATION);
	if (pDevicesLocation == NULL)
	{
		Py_XDECREF(pAppendFunc);
		throw SNPythonInterpreterException("location string", SNPythonInterpreterException::CREATE);
	}
	if (PyTuple_SetItem(pAppendParams, 0, pDevicesLocation))
	{
		Py_XDECREF(pAppendFunc);
		Py_XDECREF(pDevicesLocation);
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::SET);
	}
	PyObject_Call(pAppendFunc, pAppendParams, NULL);
	Py_XDECREF(pAppendFunc);
	Py_XDECREF(pAppendParams);

	// natiahneme globalnu premennu devices
	pMainName = PyString_FromString("__main__");
	pMainModule = PyImport_Import(pMainName);
	Py_XDECREF(pMainName);
	if (pMainModule == NULL)
	{
		throw SNPythonInterpreterException("__main__", SNPythonInterpreterException::IMPORT);
	}
	pDevicesDict = PyDict_New();
	if (pDevicesDict == NULL)
	{
		throw SNPythonInterpreterException("devices", SNPythonInterpreterException::CREATE);
	}
	if (PyObject_SetAttrString(pMainModule, "devices", pDevicesDict))
	{
		Py_XDECREF(pDevicesDict);
		throw SNPythonInterpreterException("devices", SNPythonInterpreterException::SET);
	}
	Py_XDECREF(pDevicesDict);
	for (int i = 1; i < argc; ++i)
	{
		SNDevice test(argv[i], i);
	}
*/
	Py_Finalize();
}
