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

#include "core/PyCPPObject.h"
#include "core/SNExceptions.h"
#include "config.h"

void interpreterInit()
{
	PyCPPObject pOsModuleName(PyString_FromString("os"));
	PyCPPObject pOsModule(PyImport_Import(pOsModuleName));
	PyCPPObject pPathObject(PySys_GetObject("path"));
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
	PyCPPObject pDevicesDict(PyDict_New(), true);
	if (PyObject_SetAttrString(pMainModule, "devices", pDevicesDict))
	{
		throw SNPythonInterpreterException("devices", SNPythonInterpreterException::SET);
	}
}
