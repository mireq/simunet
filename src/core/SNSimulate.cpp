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
	m_mainThreadState = PyThreadState_Get();
	PyEval_ReleaseLock();

	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);

	PyRun_SimpleString("import gc");
	PyRun_SimpleString("gc.set_debug(gc.DEBUG_LEAK)");


	setPath();
	createDevicesDictionary();
	createSNSimulateModule();
	createBaseClass();

	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();

	m_threadCount = threads;
	m_nextDeviceId = 1;

	for (int i = 0; i < m_threadCount; ++i)
	{
		SNSimulateHelper *helper = new SNSimulateHelper(m_mainThreadState);
		m_simulateHelpers.push_back(helper);
		helper->start();
	}
	m_nextSimulateHelper = m_simulateHelpers.begin();

}


SNSimulate::~SNSimulate()
{
	map<int, pair<int, SNDevice*> >::iterator dev;
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	while (!m_devices.empty())
	{
		dev = m_devices.begin();
		delete dev->second.second;
		m_devices.erase(dev);
	}
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
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
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	PyRun_SimpleString("print(gc.garbage)");
	Py_Finalize();
}


/*!
    \fn SNSimulate::stopDevice(uint32_t id)
 */
bool SNSimulate::stopDevice(uint32_t id)
{
	map<int, pair<int, SNDevice*> >::iterator dev;
	dev = m_devices.find(id);
	if (dev == m_devices.end())
	{
		return true;
	}
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	delete dev->second.second;
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();

	map<int, vector<int> >::iterator subtree;

	int subtreeId = dev->second.first;
	// vyhladame podstrom v ktorom sa nachadza zariadenie
	subtree = m_devicesTree.find(dev->second.first);

	// odstranime zariadenie z hlavneho zoznamu
	m_devices.erase(dev);

	// ak sa zariadenie nenachadza v podstrome doslo k chybe konzistencie dat
	if (subtree == m_devicesTree.end())
	{
		qWarning("Missing subtree: %d", subtreeId);
		return false;
	}

	vector<int>::iterator subtreeDev = find(subtree->second.begin(), subtree->second.end(), (int)id);
	if (subtreeDev == subtree->second.end())
	{
		qWarning("Missing device: %d in subtree %d", id, subtreeId);
		return false;
	}
	subtree->second.erase(subtreeDev);

	return false;
}


/*!
    \fn SNSimulate::startDevice(const string &filename)
 */
uint32_t SNSimulate::startDevice(const string &filename)
{
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	SNDevice *device = new SNDevice(filename, m_nextDeviceId, this);
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();

	(*m_nextSimulateHelper)->addDevice(device);

	m_devices[m_nextDeviceId] = pair<int, SNDevice*>(0, device);
	m_devicesTree[0].push_back(m_nextDeviceId);

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
void SNSimulate::frameResponse(uint32_t id, PyObject *data)
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


SNDevice *SNSimulate::device(uint32_t id) const
{
	map<int, pair<int, SNDevice*> >::const_iterator dev;
	dev = m_devices.find(id);
	if (dev == m_devices.end())
	{
		return NULL;
	}
	return dev->second.second;
}

const vector<int> *SNSimulate::devicesList(int parent) const
{
	map<int, vector<int> >::const_iterator subtree;
	subtree = m_devicesTree.find(parent);
	if (subtree == m_devicesTree.end())
	{
		return NULL;
	}
	return &subtree->second;
}

int SNSimulate::findIndexOfDevice(int devId, int parent) const
{
	map<int, vector<int> >::const_iterator subtree;
	subtree = m_devicesTree.find(parent);
	if (subtree == m_devicesTree.end())
	{
		return -1;
	}
	size_t pos = find(subtree->second.begin(), subtree->second.end(), devId) - subtree->second.begin();
	if (pos < subtree->second.size())
	{
		return pos;
	}
	else
	{
		return -1;
	}
}

void SNSimulate::move(int devId, int row, int parent)
{
	int dev1Parent = m_devices[devId].first;

	map<int, vector<int> >::iterator subtree1 = m_devicesTree.find(dev1Parent);
	map<int, vector<int> >::iterator subtree2 = m_devicesTree.find(parent);

	vector<int>::iterator dev1 = find(subtree1->second.begin(), subtree1->second.end(), devId);
	vector<int>::iterator dev2 = subtree2->second.begin() + row;

	subtree1->second.erase(dev1);
	if (dev1Parent == parent && dev1 < dev2)
	{
		subtree2->second.insert(--dev2, devId);
	}
	else
	{
		subtree2->second.insert(dev2, devId);
	}
}
