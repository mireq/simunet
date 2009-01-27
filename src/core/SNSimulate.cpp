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

#include "SNSimulate.h"
#include "SNSimulateHelper.h"
#include "SNDevice.h"
#include "PyCPPObject.h"
#include "SNExceptions.h"
#include "config.h"

#include <iostream>

using namespace std;

/*!
    \fn SNSimulate::SNSimulate()
    Vytvorenie n vlakien SNSimulateHelper
*/
SNSimulate::SNSimulate(int threads)
{
	Py_Initialize();

	// inicializacia vlakien
	PyEval_InitThreads();

	PyRun_SimpleString("import gc");
	PyRun_SimpleString("gc.set_debug(gc.DEBUG_LEAK)");

	setPath();
	createDevicesDictionary();
	createSNSimulateModule();
	createBaseClass();

	m_threadCount = threads;
	m_nextDeviceId = 1;

	for (int i = 0; i < m_threadCount; ++i)
	{
		SNSimulateHelper *helper = new SNSimulateHelper();
		m_simulateHelpers.push_back(helper);
		helper->start();
	}
	m_nextSimulateHelper = m_simulateHelpers.begin();
}


SNSimulate::~SNSimulate()
{
	map<int, SNDevice*>::iterator dev;
	while (!m_devices.empty())
	{
		dev = m_devices.begin();
		delete dev->second;
		m_devices.erase(dev);
	}
	list<SNSimulateHelper *>::iterator helper;
	for (helper = m_simulateHelpers.begin(); helper != m_simulateHelpers.end(); ++helper)
	{
		(*helper)->stop();
	}
	for (helper = m_simulateHelpers.begin(); helper != m_simulateHelpers.end(); ++helper)
	{
		(*helper)->wait(1000);
	}
	while (!m_simulateHelpers.empty())
	{
		helper = m_simulateHelpers.begin();
		delete *helper;
		m_simulateHelpers.erase(helper);
	}
	PyRun_SimpleString("print(gc.garbage)");
	PyEval_AcquireLock();
	Py_Finalize();
}


/*!
    \fn SNSimulate::stopDevice(uint32_t id)
 */
bool SNSimulate::stopDevice(uint32_t id)
{
	map<int, SNDevice*>::iterator dev;
	dev = m_devices.find(id);
	if (dev == m_devices.end())
	{
		return true;
	}
	delete dev->second;
	m_devices.erase(id);
	return false;
}


/*!
    \fn SNSimulate::startDevice(const string &filename)
 */
uint32_t SNSimulate::startDevice(const string &filename)
{
	SNDevice *device = new SNDevice(filename, m_nextDeviceId, this);
	(*m_nextSimulateHelper)->addDevice(device);
	m_devices[m_nextDeviceId] = device;
	m_nextDeviceId++;
	m_nextSimulateHelper++;
	if (m_nextSimulateHelper == m_simulateHelpers.end())
	{
		m_nextSimulateHelper = m_simulateHelpers.begin();
	}
	return m_nextDeviceId - 1;
}


/*!
    \fn SNSimulate::processFrame(int id, PyObject *data)
 */
void SNSimulate::processFrame(uint32_t id, PyObject *data)
{
    /// @todo implement me
}


/*!
    \fn SNSimulate::telnetResponse(uint32_t id, const char *text, const char *cmd)
 */
void SNSimulate::telnetResponse(uint32_t id, const char *text, const char *cmd)
{
    /// @todo implement me
}


/*!
    \brief Nastavime cesty v ktorych su hladane moduly.
 */
void SNSimulate::setPath()
{
		// ziskame referenciu na modul os
	PyCPPObject pOsModuleName(PyString_FromString("os"));
	PyCPPObject pOsModule(PyImport_Import(pOsModuleName));

	// ziskame referenciu na funkciu sys.path.append
	
	char pathName[5] = "path";
	PyCPPObject pPathObject(PySys_GetObject(pathName));
	PyCPPObject pAppendFunc(PyObject_GetAttrString(pPathObject, "append"));
	if (!pAppendFunc.isCallable())
	{
		throw SNPythonInterpreterException("append", SNPythonInterpreterException::CALL);
	}

	// ziskame referenciu na funkciu na spajanie ciest
	PyCPPObject pPathModule(PyObject_GetAttrString(pOsModule, "path"));
	PyCPPObject pJoinFunc(PyObject_GetAttrString(pPathModule, "join"));
	if (!pJoinFunc.isCallable())
	{
		throw SNPythonInterpreterException("join", SNPythonInterpreterException::CALL);
	}

	// ziskame referenciu na os.getcwd
	PyCPPObject pGetCwdFunc(PyObject_GetAttrString(pOsModule, "getcwd"));
	if (!pGetCwdFunc.isCallable())
	{
		throw SNPythonInterpreterException("getcwd", SNPythonInterpreterException::CALL);
	}

	// zavolame os.getcwd
	PyCPPObject pGetCwdParams(PyTuple_New(0));
	PyCPPObject pCwd(PyObject_Call(pGetCwdFunc, pGetCwdParams, NULL));

	// zavolame os.path.join(cwd, "devices")
	PyCPPObject pDevicesDir(PyString_FromString("devices"));
	PyCPPObject pJoinParams(PyTuple_New(2));
	if (PyTuple_SetItem(pJoinParams, 0, pCwd))
	{
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::SET);
	}
	if (PyTuple_SetItem(pJoinParams, 1, pDevicesDir))
	{
		throw SNPythonInterpreterException("args tuple", SNPythonInterpreterException::SET);
	}
	PyCPPObject pDevicesFullDir(PyObject_Call(pJoinFunc, pJoinParams, NULL));

	// zavolame sys.path.append a pridame do path aktualny adresar
	PyCPPObject pAppendParams(PyTuple_New(1));
	if (PyTuple_SetItem(pAppendParams, 0, pDevicesFullDir))
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
void SNSimulate::createDevicesDictionary()
{
	// vytvorime globalnu premennu devices
	PyCPPObject pMainName(PyString_FromString("__main__"));
	PyCPPObject pMainModule(PyImport_Import(pMainName));
	PyCPPObject pDevicesDict(PyDict_New());
	//m_pDevicesDict = pDevicesDict;
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
void SNSimulate::createSNSimulateModule()
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
void SNSimulate::createBaseClass()
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




