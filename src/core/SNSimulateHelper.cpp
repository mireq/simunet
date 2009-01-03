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
#include "SNSimulateHelper.h"

#include <Python.h>
#include <unistd.h>
#include <iostream>
#include "SNExceptions.h"
#include "PyCPPObject.h"
#include "config.h"

SNSimulateHelper::SNSimulateHelper()
	:m_stop(false)
{

	Py_Initialize();
	// ziskame referenciu na modul sys
	PyCPPObject pSysModuleName(PyString_FromString("sys"));
	PyCPPObject pSysModule(PyImport_Import(pSysModuleName));

	// ziskame referenciu na modul os
	PyCPPObject pOsModuleName(PyString_FromString("os"));
	PyCPPObject pOsModule(PyImport_Import(pOsModuleName));

	// ziskame referenciu na funkciu sys.path.append
	PyCPPObject pPathObject(PyObject_GetAttrString(pSysModule, "path"));
	PyCPPObject pAppendFunc(PyObject_GetAttrString(pPathObject, "append"));
	if (!pAppendFunc.isCallable())
	{
		throw new SNPythonInterpreterException("append", SNPythonInterpreterException::CALL);
	}

	// ziskame referenciu na os.getcwd
	PyCPPObject pGetCwdFunc(PyObject_GetAttrString(pOsModule, "getcwd"));
	if (!pGetCwdFunc.isCallable())
	{
		throw new SNPythonInterpreterException("getcwd", SNPythonInterpreterException::CALL);
	}

	// zavolame os.getcwd
	PyCPPObject pGetCwdParams(PyTuple_New(0));
	PyCPPObject pCwd(PyObject_Call(pGetCwdFunc, pGetCwdParams, NULL));

	// zavolame sys.path.append a pridame do path aktualny adresar
	PyCPPObject pAppendParams(PyTuple_New(1));
	if (PyTuple_SetItem(pAppendParams, 0, pCwd))
	{
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::SET);
	}
	PyObject_Call(pAppendFunc, pAppendParams, NULL);

	// pridame do path DEVICES_LOCATION
	PyCPPObject pDevicesLocation(PyString_FromString(DEVICES_LOCATION));
	if (PyTuple_SetItem(pAppendParams, 0, pDevicesLocation))
	{
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::SET);
	}
	PyObject_Call(pAppendFunc, pAppendParams, NULL);

	// vytvorime globalnu premennu devices
	PyCPPObject pMainName(PyString_FromString("__main__"));
	PyCPPObject pMainModule(PyImport_Import(pMainName));
	PyCPPObject pDevicesDict(PyDict_New());
	pDevicesDict.keepRef();
	m_pDevicesDict = pDevicesDict;
	if (PyObject_SetAttrString(pMainModule, "devices", pDevicesDict))
	{
		throw SNPythonInterpreterException("devices", SNPythonInterpreterException::SET);
	}
	
	/*PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
	PyRun_SimpleString("sys.path.append(os.getcwd())");
	PyRun_SimpleString("devices = {}");*/
}


SNSimulateHelper::~SNSimulateHelper()
{
	std::cout<<"Ukoncujem beh"<<std::endl;
	Py_XDECREF(m_pDevicesDict);
	Py_Finalize();
}

void SNSimulateHelper::run()
{
	while(1)
	{
		std::cout<<"Vlakno"<<std::endl;
		if (m_stop)
		{
			return;
		}
		sleep(1);
	}
}

void SNSimulateHelper::stop()
{
	m_stop = true;
}

void SNSimulateHelper::addDevice(SNDevice *device)
{
	m_devices.push_back(device);
}


