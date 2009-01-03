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
		PyCPPObject pModule(PyImport_Import(pModuleName));

		// natiahneme triedu reprezentujucu zariadenie
		PyCPPObject pDeviceClass(PyObject_GetAttrString(pModule, filename.c_str()));

		// odtestujem ci sme skutocne nacitali triedu
		if (!pDeviceClass.isClass())
		{
			throw SNDeviceImportException(filename);
		}

		// natiahneme globalnu premennu devices
		PyCPPObject pMainModuleName(PyString_FromString("__main__"));
		PyCPPObject pMainModule(PyImport_Import(pMainModuleName));
		PyCPPObject pDevicesDict(PyObject_GetAttrString(pMainModule, "devices"));

		// vytvorime novu instanciu zariadenia
		PyCPPObject pDeviceInstance(PyInstance_New(pDeviceClass, NULL, NULL));
		PyCPPObject pDeviceId(PyInt_FromLong(deviceId));

		// a ulozime ju do asociativneho pola
		PyDict_SetItem(pDevicesDict, pDeviceId, pDeviceInstance);
	}
	catch (PyObjectNULLException e)
	{
		throw SNDeviceImportException(filename);
	}
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
