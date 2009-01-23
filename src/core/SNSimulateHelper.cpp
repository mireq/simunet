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
#include "SNDevice.h"

#include <unistd.h>
#include <iostream>
#include "SNExceptions.h"
#include "PyCPPObject.h"
#include "config.h"

SNSimulateHelper::SNSimulateHelper()
	:m_stop(false)
{

	Py_Initialize();
	setPath();
	createDevicesDictionary();
	createSNSimulateModule();
	createBaseClass();
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

/*!
    \brief Nastavime cesty v ktorych su hladane moduly.
  */
void SNSimulateHelper::setPath()
{
		// ziskame referenciu na modul os
	PyCPPObject pOsModuleName(PyString_FromString("os"));
	PyCPPObject pOsModule(PyImport_Import(pOsModuleName));

	// ziskame referenciu na funkciu sys.path.append
	char funcName[5] = "path";
	PyCPPObject pPathObject(PySys_GetObject(funcName));
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
}

/*!
    \brief Vytvorenie pythonovskej struktury v ktorej budu udrziavane zariadenia.
  */
void SNSimulateHelper::createDevicesDictionary()
{
	// vytvorime globalnu premennu devices
	PyCPPObject pMainName(PyString_FromString("__main__"));
	PyCPPObject pMainModule(PyImport_Import(pMainName));
	PyCPPObject pDevicesDict(PyDict_New());
	m_pDevicesDict = pDevicesDict;
	if (PyObject_SetAttrString(pMainModule, "devices", pDevicesDict))
	{
		throw SNPythonInterpreterException("devices", SNPythonInterpreterException::SET);
	}
}

/*!
    \brief Vytvorenie prepojenia na c++ triedu SNSimulate
    Tato funkcia vytvori pythonovsky modul snsimulate ktory sluzi na komunikaciu
    pythonu s c-rozhranim. Tento modul sa nebude vyuzivat priamo, ale iba
    z triedy SNDevice cez metody ako sendFrame.
 */
void SNSimulateHelper::createSNSimulateModule()
{
	Py_InitModule("snsimulate", const_cast<PyMethodDef *>(SNDevice::SNSimulateMethods));
	PyCPPObject pSNSimulateName(PyString_FromString("snsimulate"));
	PyCPPObject pSNSimulateModule(PyImport_Import(pSNSimulateName));
	PyCPPObject pBuiltinsDict(PyEval_GetBuiltins());
	PyDict_SetItemString(pBuiltinsDict, "snsimulate", pSNSimulateModule);
}

/*!
    \brief Vytvorenie zakladnej pythonovskej triedy pre zariadenia (SNDevice)
  */
void SNSimulateHelper::createBaseClass()
{
	PyCPPObject pBuiltinsDict(PyEval_GetBuiltins());
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
}


