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
#include "map/SNMap.h"
#include "config.h"

#include <iostream>
#include <algorithm>
#include <QFile>
#include <QDebug>

using namespace std;

/*!
  \class SNSimulate
  \brief Trieda zabezpecujuca samotnu simulaciu.
  \ingroup core

  Tato trieda obsahuje odkazy na vsetky zariadenia. Pri vytvoreni objektu
  sa vytvori niekolko objektov typu SNSimulateHelper (podla nastavenia)
  poctu vlakien.

  \sa SNSimulateHelper
*/

/*!
  Vytvorenie n vlakien SNSimulateHelper
*/
SNSimulate::SNSimulate(int threads)
	: m_map(NULL)
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
	createSNSimulateModule();
	createDevicesDictionary();
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

/*!
  Ukoncenie vsetkych vlakien.
*/
SNSimulate::~SNSimulate()
{
	map<uint32_t, SNDevice*>::iterator dev;
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	while (!m_devices.empty())
	{
		dev = m_devices.begin();
		delete dev->second;
		m_devices.erase(dev);
	}
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
	vector<SNSimulateHelper *>::iterator helper;
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
 \brief Odstranenie zariadenia zo simulacie.
 \param id Unikatne identifikacne cislo zariadenia.
 \return Po uspesnom odstraneni je navratova hodnota false, pri chybe true.
 */
bool SNSimulate::stopDevice(uint32_t id)
{
	map<uint32_t, SNDevice*>::iterator dev;
	dev = m_devices.find(id);
	if (dev == m_devices.end())
	{
		qWarning("Missing device: %d", id);
		return true;
	}
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	delete dev->second;
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();

	m_devices.erase(dev);
	return false;
}

/*!
  \brief Pridanie zariadenia do simulacie.
  \param filename Zariadenie ktore sa ma nastartovat.
  \return Navratovou hodnotou je jedinecne identifikacne cislo zariadenia.
 */
uint32_t SNSimulate::startDevice(const std::string &filename)
{
	// start zariadenia v pythonovi
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	SNDevice *device = new SNDevice(filename, m_nextDeviceId, this);
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();

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
  \brief Spracovanie ramca / preposlanie inym zariadeniam.
  \param id ID zariadenia.
  \param data Data ktore poslalo zariadenie.

  Tato metoda zachytava spravy od zariadeni a spracuje ich / preposiela
  ostatnym zariadeniam.
 */
void SNSimulate::frameResponse(uint32_t id, PyObject *data)
{
    /// @todo implement me
}

/*!
  \brief Prijatie telnet odpovede od zariadenia.
  \param id ID zariadenia.
  \param text Text ktory vypisalo zariadenie.
  \param cmd Text ktory sa ma pouzit ako prompt.
 */
void SNSimulate::telnetResponse(uint32_t id, const char *text, const char *cmd)
{
	emit telnetResponseRecived(id, text, cmd);
}

/*!
  Odoslanie HTTP požiadavky zariadeniu špecifikovanému atribútom \a devId.
  V prípade, že toto zariadenie neexistuje bude návratovou hodnotou NULL.

  \sa SNDevice::httpRequest
*/
char *SNSimulate::httpRequest(uint32_t devId, const std::string &url, PyObject *post)
{
	SNDevice *dev = device(devId);
	if (dev == NULL)
	{
		return NULL;
	}
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	char *out = dev->httpRequest(url, post);
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
	return out;
}

/*!
  \overload
*/
char *SNSimulate::httpRequest(uint32_t devId, const std::string &url, const std::map<std::string, std::string> post)
{
	SNDevice *dev = device(devId);
	if (dev == NULL)
	{
		return NULL;
	}
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	char *out = dev->httpRequest(url, post);
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
	return out;
}

/*!
  Odoslanie telnet požiadavky vybranému zariadeniu. Ak zariadenie s daným
  \a devId neexistuje váti táto funkcia NULL.

  \sa SNDevice::telnetRequest
*/
char *SNSimulate::telnetRequest(uint32_t devId, const std::string &line, char symbol)
{
	SNDevice *dev = device(devId);
	if (dev == NULL)
	{
		return NULL;
	}
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	char *out = dev->telnetRequest(line, symbol);
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
	return out;
}

/*!
  Odosllanie ramca zariadeni urcenemu argumentom \a targetDevId.
*/
void SNSimulate::sendFrame(uint32_t targetDevId, PyObject *frame)
{
	m_simulateHelpers[targetDevId % m_threadCount]->sendFrame(targetDevId, frame);
}

/*!
  Tato metoda sa vola priamo zo zariadenia pri pridani hardwaroveho portu na
  zariadeni.

  \sa hwPortRemoved
*/
void SNSimulate::hwPortInserted(uint32_t devId, port_num hwPort)
{
	if (m_map == NULL || m_devices.find(devId) == m_devices.end())
	{
		return;
	}
	m_map->insertPort(devId, hwPort);
}

/*!
  Tato metoda sa vola pri odstraneni hardwaroveho portu zo zariadenia.

  \sa hwPortInserted
*/
void SNSimulate::hwPortRemoved(uint32_t devId, port_num hwPort)
{
	if (m_map == NULL || m_devices.find(devId) == m_devices.end())
	{
		return;
	}
	m_map->removePort(devId, hwPort);
}

void SNSimulate::setMap(SNMap *map)
{
	m_map = map;
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
	PyCPPObject pMainModule(PyImport_AddModule("snsimulate"));
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
	PyCPPObject pSimuNetModule(PyImport_AddModule("SimuNet"));
	PyCPPObject pSimuNetDict(PyModule_GetDict(pSimuNetModule));
	if (PyDict_GetItemString(pSimuNetDict, "__builtins__") == NULL)
	{
		PyCPPObject builtins(PyImport_ImportModule("__builtin__"), true);
		PyDict_SetItemString(pSimuNetDict, "__builtins__", builtins);
	}


	QFile file(":/devices/baseDevice/device.py");
	file.open(QIODevice::ReadOnly);
	QByteArray data = file.readAll();
	file.close();
	PyCPPObject ret(PyRun_String(data.constData(), Py_single_input, pSimuNetDict, pSimuNetDict), true);

	file.setFileName(":/devices/baseDevice/simulatehelper.py");
	file.open(QIODevice::ReadOnly);
	data = file.readAll();
	file.close();
	PyCPPObject ret2(PyRun_String(data.constData(), Py_single_input, pSimuNetDict, pSimuNetDict), true);
}

/*!
 \brief Ziskanie referencie na zariadenie
 \param id ID zariadenia
 \return Navratovou hodotou je referencia zariadenia ak existuje, NULL ak neexistuje.
 */
SNDevice *SNSimulate::device(uint32_t id) const
{
	map<uint32_t, SNDevice*>::const_iterator dev;
	dev = m_devices.find(id);
	if (dev == m_devices.end())
	{
		return NULL;
	}
	return dev->second;
}


/*!
  \fn SNSimulate::telnetResponseRecived(uint32_t id, const char *text, const char *cmd)

  Tento signal sa emituje po prijati telnet dat od zariadenia.
*/
