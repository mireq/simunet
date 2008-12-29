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

#include <iostream>

using namespace std;

SNDevice::SNDevice(const string &filename, uint32_t deviceId)
{
	PyObject *pModuleName, *pModule, *pDeviceClass, *pDevicesDict, *pGlobals;
	PyObject *pDeviceId, *pDeviceInstance, *pMainModule;

	// import modulu, modul je vzdy v adresari s rovnakym nazvom preto modu.modul
	pModuleName = PyString_FromString(string(filename + "." + filename).c_str());
	pModule = PyImport_Import(pModuleName);
	Py_XDECREF(pModuleName);
	if (pModule == NULL)
	{
		throw SNDeviceImportException(filename);
	}

	// natiahneme triedu reprezentujucu zariadenie
	pDeviceClass = PyObject_GetAttrString(pModule, filename.c_str());
	Py_XDECREF(pModule);
	if (pDeviceClass == NULL)
	{
		throw SNDeviceImportException(filename);
	}

	// odtestujem ci sme skutocne nacitali triedu
	if (!PyClass_Check(pDeviceClass))
	{
		Py_XDECREF(pDeviceClass);
		throw SNDeviceImportException(filename);
	}

	// natiahneme globalnu premennu devices
	pModuleName = PyString_FromString("__main__");
	pMainModule = PyImport_Import(pModuleName);
	Py_XDECREF(pModuleName);
	if (pMainModule == NULL)
	{
		Py_XDECREF(pDeviceClass);
		throw SNDeviceImportException(filename);
	}
	pDevicesDict = PyObject_GetAttrString(pMainModule, "devices");
	Py_XDECREF(pMainModule);
	if (pDevicesDict == NULL)
	{
		Py_XDECREF(pDeviceClass);
		throw SNDeviceImportException(filename);
	}

	// vytvorime novu instanciu zariadenia
	pDeviceInstance = PyInstance_New(pDeviceClass, NULL, NULL);
	Py_XDECREF(pDeviceClass);
	if (pDeviceInstance == NULL)
	{
		Py_XDECREF(pDeviceId);
		Py_XDECREF(pDevicesDict);
		throw SNDeviceImportException(filename);
	}
	pDeviceId = PyInt_FromLong(deviceId);

	// a ulozime ju do asociativneho pola
	PyDict_SetItem(pDevicesDict, pDeviceId, pDeviceInstance);
	Py_XDECREF(pDeviceId);
	Py_XDECREF(pDevicesDict);
}


SNDevice::~SNDevice()
{
}


/*!
    \fn SNDevice::processFrame(data *PyObject)
 */
bool SNDevice::processFrame(PyObject *data)
{
    /// @todo implement me
}


/*!
    \fn SNDevice::resetConfig(void)
 */
bool SNDevice::resetConfig(void)
{
    /// @todo implement me
}


/*!
    \fn SNDevice::setConfig(PyObject *data)
 */
bool SNDevice::setConfig(PyObject *data)
{
    /// @todo implement me
}


/*!
    \fn SNDevice::dumpConfig(void)
 */
PyObject * SNDevice::dumpConfig(void)
{
    /// @todo implement me
}


/*!
    \fn SNDevice::httpRequest(const string &url, PyObject *post)
 */
string SNDevice::httpRequest(const string &url, PyObject *post)
{
    /// @todo implement me
}


/*!
    \fn SNDevice::telnetRequest(const string &line, char symbol)
 */
string SNDevice::telnetRequest(const string &line, char symbol)
{
    /// @todo implement me
}


/*!
    \fn SNDevice::telnetGetControlChars(void)
 */
char * SNDevice::telnetGetControlChars(void)
{
    /// @todo implement me
}
