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
#include "SNDevice.h"
#include "SNExceptions.h"
#include "PyCPPObject.h"

#include <iostream>

using namespace std;

SNDevice::SNDevice(const string &filename, uint32_t deviceId)
{
	try
	{
		// import modulu, modul je vzdy v adresari s rovnakym nazvom preto modu.modul
		PyCPPObject pModuleName(PyString_FromString(string(filename + "." + filename).c_str()));
		PyCPPObject pModule(PyImport_Import(pModuleName), false);

		// natiahneme triedu reprezentujucu zariadenie
		PyCPPObject pDeviceClass(PyObject_GetAttrString(pModule, filename.c_str()), false);

		// odtestujem ci sme skutocne nacitali triedu
		if (!pDeviceClass.isClass())
		{
			throw SNDeviceImportException(filename);
		}

		// natiahneme globalnu premennu devices
		PyCPPObject pMainModuleName(PyString_FromString("__main__"));
		PyCPPObject pMainModule(PyImport_Import(pMainModuleName));
		PyCPPObject pDevicesDict(PyObject_GetAttrString(pMainModule, "devices"), true);

		// vytvorime novu instanciu zariadenia
		PyCPPObject pDeviceInstance(PyInstance_New(pDeviceClass, NULL, NULL), true);
		m_pDeviceInstance = pDeviceInstance;
		PyCPPObject pDeviceId(PyLong_FromUnsignedLong(deviceId));

		// a ulozime ju do asociativneho pola
		PyDict_SetItem(pDevicesDict, pDeviceId, pDeviceInstance);
		PyObject_SetAttrString(pDeviceInstance, "device_id", pDeviceId);
	}
	catch (PyObjectNULLException e)
	{
		throw SNDeviceImportException(filename);
	}
	m_deviceId = deviceId;
}


SNDevice::~SNDevice()
{
}


/*!
    \fn SNDevice::processFrame(data *PyObject)
 */
bool SNDevice::processFrame(PyObject *data)
{
	PyCPPObject pProcessFrameFunc(PyObject_GetAttrString(m_pDeviceInstance, "processFrame"), true);
	if (!pProcessFrameFunc.isCallable())
	{
		return false;
	}
	PyCPPObject args(PyTuple_New(1));
	PyTuple_SetItem(args, 0, data);
	PyCPPObject ret(PyObject_Call(pProcessFrameFunc, args, NULL));
	if (!PyBool_Check(ret))
	{
		return false;
	}
	else
	{
		return PyObject_IsTrue(ret);
	}
	return false;
}


/*!
    \fn SNDevice::resetConfig(void)
 */
bool SNDevice::resetConfig(void)
{
	try
	{
		PyCPPObject resetConfigFunc(PyObject_GetAttrString(m_pDeviceInstance, "resetConfig"));
		if (!resetConfigFunc.isCallable())
		{
			return false;
		}
		PyCPPObject ret(PyObject_Call(resetConfigFunc, NULL, NULL));
		return (PyObject_IsTrue(ret) == 1);
	}
	catch (PyObjectNULLException e)
	{
		return false;
	}
}


/*!
    \fn SNDevice::setConfig(PyObject *data)
 */
bool SNDevice::setConfig(PyObject *data)
{
	try
	{
		PyCPPObject setConfigFunc(PyObject_GetAttrString(m_pDeviceInstance, "setConfig"));
		if (!setConfigFunc.isCallable())
		{
			return false;
		}
		PyCPPObject args(PyTuple_New(1));
		PyTuple_SetItem(args, 0, data);
		PyCPPObject ret(PyObject_Call(setConfigFunc, args, NULL));
		return (PyObject_IsTrue(ret) == 1);
	}
	catch (PyObjectNULLException e)
	{
		return false;
	}
}


/*!
    \fn SNDevice::dumpConfig(void)
 */
PyObject *SNDevice::dumpConfig(void)
{
	try
	{
		PyCPPObject dumpConfigFunc(PyObject_GetAttrString(m_pDeviceInstance, "dumpConfig"));
		if (!dumpConfigFunc.isCallable())
		{
			return NULL;
		}
		return PyObject_Call(dumpConfigFunc, NULL, NULL);
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}


/*!
    \fn SNDevice::httpRequest(const string &url, PyObject *post)
 */
char *SNDevice::httpRequest(const string &url, PyObject *post)
{
	try
	{
		PyCPPObject httpRequestFunc(PyObject_GetAttrString(m_pDeviceInstance, "httpRequest"));
		if (!httpRequestFunc.isCallable())
		{
			return NULL;
		}
		PyCPPObject pUrl(PyString_FromString(url.c_str()));
		PyCPPObject args(PyTuple_New(2));

		PyTuple_SetItem(args, 0, pUrl);
		PyTuple_SetItem(args, 1, post);
		PyCPPObject pRet(PyObject_Call(httpRequestFunc, args, NULL));
		return PyString_AsString(pRet);
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}


/*!
    \fn SNDevice::telnetRequest(const string &line, char symbol)
 */
char *SNDevice::telnetRequest(const string &line, char symbol)
{
	try
	{
		PyCPPObject telnetRequestFunc(PyObject_GetAttrString(m_pDeviceInstance, "telnetRequest"));
		if (!telnetRequestFunc.isCallable())
		{
			return NULL;
		}
		PyCPPObject args(PyTuple_New(2));

		PyCPPObject pLine(PyString_FromString(line.c_str()));
		PyCPPObject pSymbol(PyString_FromFormat("%c", symbol));

		PyTuple_SetItem(args, 0, pLine);
		PyTuple_SetItem(args, 1, pSymbol);

		PyCPPObject pRet(PyObject_Call(telnetRequestFunc, args, NULL));
		return PyString_AsString(pRet);
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}


/*!
    \fn SNDevice::telnetGetControlChars(void)
 */
char *SNDevice::telnetGetControlChars(void)
{
	try
	{
		PyCPPObject telnetGetControlCharsFunc(PyObject_GetAttrString(m_pDeviceInstance, "telnetGetControlChars"));
		if (!telnetGetControlCharsFunc.isCallable())
		{
			return NULL;
		}
		PyCPPObject pRet(PyObject_Call(telnetGetControlCharsFunc, NULL, NULL));
		return PyString_AsString(pRet);
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}
