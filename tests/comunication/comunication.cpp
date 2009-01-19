/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2008 by Miroslav Bendik                                 *
 *   miroslav.bendik@gmail.com                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "core/SNDevice.h"
#include "core/PyCPPObject.h"
#include "shared.h"
#include <Python.h>

#include <iostream>

using namespace std;


PyObject *processFrameWrapper(PyObject *self, PyObject *args)
{
	if (PyTuple_Size(args) != 3)
	{
		return NULL;
	}

	PyCPPObject pSNDeviceInstance(PyTuple_GetItem(args, 0));
	PyCPPObject pDeviceId(PyTuple_GetItem(args, 1));
	PyCPPObject pData(PyTuple_GetItem(args, 2));

	uint32_t deviceId = PyLong_AsUnsignedLong(pDeviceId);
	SNDevice *dev = (SNDevice *)PyCObject_AsVoidPtr(pSNDeviceInstance);
	cout<<"id:"<<deviceId<<endl;
	cout<<"data:"<<PyString_AsString(pData)<<endl;
	dev->telnetRequest("line", 'c');
	/*if (dev->m_simulate != NULL)
	{
		dev->m_simulate->processFrame(deviceId, pData);
	}*/

	Py_RETURN_NONE;
}

PyObject* telnetResponseWrapper(PyObject *self, PyObject *args)
{
	if (PyTuple_Size(args) != 4)
	{
		return NULL;
	}

	//PyCPPObject pSNDeviceInstance(PyTuple_GetItem(args, 0));
	PyCPPObject pDeviceId(PyTuple_GetItem(args, 1));
	PyCPPObject pText(PyTuple_GetItem(args, 2));
	PyCPPObject pCmd(PyTuple_GetItem(args, 3));

	uint32_t deviceId = PyLong_AsUnsignedLong(pDeviceId);
	if (!PyString_Check(pText) || !PyString_Check(pCmd))
	{
		return NULL;
	}

	char *text = PyString_AsString(pText);
	char *cmd  = PyString_AsString(pCmd);
	cout<<"telnet:"<<deviceId<<":"<<text<<":"<<cmd<<endl;
	/*SNDevice *dev = (SNDevice *)PyCObject_AsVoidPtr(pSNDeviceInstance);
	if (dev->m_simulate != NULL)
	{
		dev->m_simulate->telnetResponse(deviceId, text, cmd);
	}*/

	Py_RETURN_NONE;
}

const PyMethodDef SNSimulateMethods[] = {
 {"sendFrame", processFrameWrapper, METH_VARARGS, "Odoslanie ramca"},
 {"sendTelnet", telnetResponseWrapper, METH_VARARGS, "Odoslanie dat cez telnet"},
 {NULL, NULL, 0, NULL}
};

int main(int argc, char *argv[])
{
	Py_Initialize();
	PyRun_SimpleString("import gc");
	PyRun_SimpleString("gc.set_debug(gc.DEBUG_LEAK)");
	interpreterInit();

	Py_InitModule("snsimulate", const_cast<PyMethodDef *>(SNSimulateMethods));
	//PyRun_SimpleString("import snsimulate");

	PyCPPObject pSNSimulateName(PyString_FromString("snsimulate"));
	PyCPPObject pSNSimulateModule(PyImport_Import(pSNSimulateName));

	//PyObject_SetAttrString(PyEval_GetGlobals(), "snsimulate", pSNSimulateModule);

	//PyCPPObject pBuiltinsDict(PyDict_New());
	PyCPPObject pBuiltinsDict(PyEval_GetBuiltins());
	PyDict_SetItemString(pBuiltinsDict, "snsimulate", pSNSimulateModule);
	PyRun_String("class SNDevice:\n"
			"\tdef sendFrame(self, data):\n"
			"\t\tsnsimulate.sendFrame(self.pSNDevice, self.deviceId, data)\n"
			"\tdef sendTelnet(self, text, cmd):\n"
			"\t\tsnsimulate.sendTelnet(self.pSNDevice, self.deviceId, text, cmd)\n"
			"\tdef processFrame(self, data):\n"
			"\t\tprint(\"processFrame not implemented\")\n"
			"\tdef resetConfig(self):\n"
			"\t\tprint(\"resetConfig not implemented\")\n"
			"\tdef setConfig(self, data):\n"
			"\t\tprint(\"setConfig not implemented\")\n"
			"\tdef dumpConfig(self):\n"
			"\t\tprint(\"dumpConfig not implemented\")\n"
			"\tdef httpRequest(self, url, post):\n"
			"\t\tprint(\"httpRequest not implemented\")\n"
			"\tdef telnetRequest(self, line, symbol):\n"
			"\t\tprint(\"telnetRequest not implemented\")\n"
			"\tdef telnetGetControlChars(self):"
			"\t\tprint(\"telnetGetControlChars not implemented\")", Py_single_input, pBuiltinsDict, pBuiltinsDict);
	for (int i = 1; i < argc; ++i)
	{
		SNDevice test(argv[i], i);
		PyCPPObject f1(PyString_FromString("frame"));
		test.processFrame(f1);
	}

	Py_Finalize();
}
