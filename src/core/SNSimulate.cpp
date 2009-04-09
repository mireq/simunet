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
#include <algorithm>
#include <QFile>

using namespace std;

/*!
  \class SNSimulate
  \brief Trieda zabezpecujuca samotnu simulaciu.
  \ingroup core

  Tato trieda obsahuje odkazy na vsetky zariadenia a zaroven obsahuje virtualnu
  adresarovu strukturu so jednotlivymi zariadeniami. Pri vytvoreni objektu
  sa vytvori niekolko objektov typu SNSimulateHelper (podla nastavenia)
  poctu vlakien.

  \sa SNSimulateHelper
*/

/*!
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
	m_nextFolderId = 1;

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
 \brief Odstranenie zariadenia zo simulacie.
 \param id Unikatne identifikacne cislo zariadenia.
 \return Po uspesnom odstraneni je navratova hodnota false, pri chybe true.
 */
bool SNSimulate::stopDevice(uint32_t id)
{
	map<int, pair<int, SNDevice*> >::iterator dev;
	dev = m_devices.find(id);
	if (dev == m_devices.end())
	{
		qWarning("Missing device: %d", id);
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
	if (subtree->second.size() == 0)
	{
		m_devicesTree.erase(subtree);
	}

	return false;
}

/*!
  \brief Pridanie zariadenia do simulacie.
  \param filename Zariadenie ktore sa ma nastartovat.
  \param directory Virtualny adresar v ktorom bude zariadenie.
  \param row Riadok na ktorom sa ma zariadenie zobrazovat.
  \return Navratovou hodnotou je jedinecne identifikacne cislo zariadenia.

  Zariadenia sa zobrazuju vo virtualnej adresarovej strukture. Ak sa tato
  funkcia zavola bez parametrov bude sa dane zariadenie zobrazovat ako posledna
  polozka v korenovom adresari. Korenovy adresar ma piradene unikatne cislo 0.

  Riadky do ktorych sa vkladaju zariadenia su cislovane od 0. Ak ponechame
  standardne nastavenie riadku (-1) zariadenie sa prida na koniec zoznamu
  zariadeni.
 */
uint32_t SNSimulate::startDevice(const std::string &filename, int directory, int row)
{
	// start zariadenia v pythonovi
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	SNDevice *device = new SNDevice(filename, m_nextDeviceId, this);
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();

	(*m_nextSimulateHelper)->addDevice(device);

	// zaradenie zariadenia do zoznamov
	// ----------------------------------------------------------------
	m_devices[m_nextDeviceId] = pair<int, SNDevice*>(directory, device);
	if (row == -1)
	{
		m_devicesTree[directory].push_back(m_nextDeviceId);
	}
	else
	{
		map<int, vector<int> >::iterator subtree = m_devicesTree.find(directory);
		if (subtree == m_devicesTree.end() || row > (int)subtree->second.size())
		{
			m_devicesTree[directory].push_back(m_nextDeviceId);
		}
		else
		{
			vector<int>::iterator dev = subtree->second.begin() + row;
			subtree->second.insert(dev, m_nextDeviceId);
		}
	}

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
	QFile file(":/devices/baseDevice/device.py");
	file.open(QIODevice::ReadOnly);
	PyCPPObject ret(PyRun_String(file.readAll().constData(), Py_single_input, pBuiltinsDict, pBuiltinsDict), true);
	file.close();
}

/*!
 \brief Ziskanie referencie na zariadenie
 \param id ID zariadenia
 \return Navratovou hodotou je referencia zariadenia ak existuje, NULL ak neexistuje.
 */
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

/*!
  \brief Vrati zoznam zariadeni vo zvolenom adresari.
*/
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

/*!
  \brief Vrati riadok na ktorom je zariadenie.

  \param devId ID zariadenia o ktorom chceme zistit na ktorom riadku je.
  \param parent ID adresara v ktorom sa ma zariadenie nachadzat.

  Tato funkcia vracia cislo riadku na ktorom je zariadenie ak sa zariadenie
  nachadza v zadanom adresari. Ak sa zariadenie nenaslo tato funkcia vrati -1-
*/
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

/*!
  \overload

  Tato metoda nevyzaduje spacifikovanie adresara. Vyhladava zariadenie vo
  vsetkych adresaroch a vrati riadok na ktorom sa nachadza.
*/
int SNSimulate::findIndexOfDevice(int devId) const
{
	int parent = 0;
	if (devId > 0)
	{
		std::map<int, std::pair<int, SNDevice*> >::const_iterator device = m_devices.find(devId);
		if (device == m_devices.end())
		{
			return -1;
		}
		else
		{
			parent = device->second.first;
		}
	}
	else
	{
		std::map<int, std::pair<int, std::string> >::const_iterator folder = m_folders.find(-devId);
		if (folder == m_folders.end())
		{
			return -1;
		}
		else
		{
			parent = folder->second.first;
		}
	}
	map<int, vector<int> >::const_iterator subtree = m_devicesTree.find(parent);
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

/*!
  \brief Najdenie nadradenej polozky v adresarovej strukture.

  \param devId ID zariadenia alebo adresara kroreho nadradenu polozku chceme zistit.
*/
int SNSimulate::parent(int devId) const
{
	if (devId > 0)
	{
		std::map<int, std::pair<int, SNDevice*> >::const_iterator dev = m_devices.find(devId);
		if (dev == m_devices.end())
		{
			return 0;
		}
		else
		{
			return dev->second.first;
		}
	}
	else
	{
		std::map<int, std::pair<int, string> >::const_iterator folder = m_folders.find(-devId);
		if (folder == m_folders.end())
		{
			return 0;
		}
		else
		{
			return folder->second.first;
		}
	}
}

/*!
  \brief Presun zariadenia alebo adresara na ine miesto.

  \param devId ID zariadenia alebo adresara ktore presuvame.
  \param row Riadok na ktory sa ma zariadenie alebo adresar presunut.
  \param parent ID adresara do ktoreho zariadenie presunieme.

  Adresar do ktoreho chceme zariadenie presunut je nepovinny. V pripade,
  ze ho vynechame bude zariadenie presunute do korenoveho adresara.
*/
void SNSimulate::move(int devId, int row, int parent)
{
	int dev1Parent = m_devices[devId].first;

	map<int, vector<int> >::iterator subtree1 = m_devicesTree.find(dev1Parent);
	map<int, vector<int> >::iterator subtree2 = m_devicesTree.find(parent);

	if (subtree1 == m_devicesTree.end() || subtree2 == m_devicesTree.end())
	{
		qWarning("Subtree not found");
	}

	vector<int>::iterator dev1 = find(subtree1->second.begin(), subtree1->second.end(), devId);
	vector<int>::iterator dev2 = subtree2->second.begin() + row;

	if (dev1 == subtree1->second.end() || dev2 == subtree2->second.end())
	{
		qWarning("Device not found");
	}

	subtree1->second.erase(dev1);
	if (dev1Parent == parent && dev1 < dev2)
	{
		subtree2->second.insert(--dev2, devId);
	}
	else
	{
		subtree2->second.insert(dev2, devId);
	}

	vector<int>::iterator subtreeDev;
}

/*!
  \brief Odstranenie zariadenia z adresara.

  Tato funkcia sa pouziva spolu s addToSubtree() namiesto funkcie move() pri
  pouziti model / view programovania.

  \warning Tato metoda sa nema pouzivat samostatne ale len spolu s funkciou
  addToSubtree(). Medzi volaniami tychto 2 metod by sa nemali zariadenia
  modifikovat.

  \sa addToSubtree() move()
*/
void SNSimulate::removeFromSubtree(int devId, int parent)
{
	map<int, vector<int> >::iterator subtree = m_devicesTree.find(parent);
	if (subtree == m_devicesTree.end())
	{
		qWarning("Subtree %d not found", parent);
		return;
	}

	vector<int>::iterator dev = find(subtree->second.begin(), subtree->second.end(), devId);
	if (dev == subtree->second.end())
	{
		qWarning("Device %d not found (subtree %d)", devId, parent);
		return;
	}
	subtree->second.erase(dev);
	if (subtree->second.size() == 0)
	{
		m_devicesTree.erase(subtree);
	}
}

/*!
  \brief Pridanie existujuceho zariadenia do adresara.

  Parametre tejto metody su identicke s move(). Tato metoda sa pouziva len
  pri model / view programovani.

  \warning Tato metoda sa pouziva po volani removeFromSubtree(). Medzi volanim
  removeFromSubtree() a addToSubtree() by sa zariadenia nemali modifikovat.

  \sa removeFromSubtree() move()
*/
void SNSimulate::addToSubtree(int devId, int row, int parent)
{
	map<int, vector<int> >::iterator subtree = m_devicesTree.find(parent);
	if (subtree == m_devicesTree.end())
	{
		m_devicesTree[parent] = vector<int>();
		subtree = m_devicesTree.find(parent);
	}

	if (row > (int)subtree->second.size())
	{
		qWarning("Device %d not found (subtree %d)", row, parent);
		return;
	}
	vector<int>::iterator dev = subtree->second.begin() + row;

	if (devId > 0)
	{
		m_devices[devId].first = parent;
	}
	else
	{
		m_folders[-devId].first = parent;
	}
	subtree->second.insert(dev, devId);
}

/*!
  \brief Vytvorenie adresara vo virtualnej adresarovej strukture.

  \param name Nazov novo vytvoreneho adresara.
  \param parent ID nadradeneho adresara.
  \param row Riadok na ktorom sa ma vytvorit novy adresar.

  Nazvy adresarov su kodovane v utf-8. Dlzka ani znaky nie su obmedzene.
  Nadradeny adresar je nepovinny, v pripade vynechania sa adresar zaradi
  do korenoveho adresara. Po vynechani riadku sa zariadenie zaradi na koniec
  zoznamu zariadeni v danom adresari.

  \sa removeDirectory()
*/
void SNSimulate::addDirectory(const std::string &name, int parent, int row)
{
	// pridanie do zoznamu adresarov
	m_folders[m_nextFolderId].first = parent;
	m_folders[m_nextFolderId].second = name;

	// pridanie do stromu
	if (row == -1)
	{
		m_devicesTree[parent].push_back(-m_nextFolderId);
	}
	else
	{
		map<int, vector<int> >::iterator subtree = m_devicesTree.find(parent);
		if (subtree == m_devicesTree.end() || row > (int)subtree->second.size())
		{
			m_devicesTree[parent].push_back(-m_nextFolderId);
		}
		else
		{
			vector<int>::iterator dev = subtree->second.begin() + row;
			subtree->second.insert(dev, -m_nextFolderId);
		}
	}

	m_nextFolderId++;
}

/*!
  \brief Premenovanie adresara.

  \param name Novy nazov adresara.
  \param directoryId ID adresra ktory chceme premenovat.

  \sa addDirectory()
*/
void SNSimulate::renameDirectory(const std::string &name, int directoryId)
{
	map<int, pair<int, string> >::iterator dir = m_folders.find(-directoryId);
	if (dir != m_folders.end())
	{
		dir->second.second = name;
	}
}

/*!
  \brief Odstranenie adresara.

  \param directoryId ID adresara ktory chceme odstranit.
  \return Zoznam zariadeni a adresarov, ktore boli odstranene.

  \warning Tato metoda odstrani adresar spolocne so vsetkymi podadresarmi
  a zariadeniami v nich.
*/
const list<int> SNSimulate::removeDirectory(int directoryId)
{
	list<int> odstranene;
	map<int, pair<int, string> >::iterator dir;
	dir = m_folders.find(-directoryId);
	if (dir == m_folders.end())
	{
		qWarning("Directory %d not found", directoryId);
		return odstranene;
	}

	// odstranenie podpoloziek
	map<int, vector<int> >::iterator folderIterator = m_devicesTree.find(directoryId);
	if (folderIterator != m_devicesTree.end())
	{
		list<int> removeList;
		for (vector<int>::iterator sf = folderIterator->second.begin(); sf != folderIterator->second.end(); ++sf)
		{
			removeList.push_back(*sf);
		}

		for (list<int>::iterator rm = removeList.begin(); rm != removeList.end(); ++rm)
		{
			if (*rm > 0)
			{
				stopDevice(*rm);
				odstranene.push_back(*rm);
			}
			else
			{
				const list<int> adresar = removeDirectory(*rm);
				odstranene.insert(--odstranene.begin(), adresar.begin(), adresar.end());
			}
		}
	}
	odstranene.push_back(directoryId);

	int subtreeId = dir->second.first;

	map<int, vector<int> >::iterator subtree = m_devicesTree.find(dir->second.first);
	m_folders.erase(dir);

	// ak sa zariadenie nenachadza v podstrome doslo k chybe konzistencie dat
	if (subtree == m_devicesTree.end())
	{
		qWarning("Missing subtree: %d", subtreeId);
		return odstranene;
	}

	vector<int>::iterator subtreeDir = find(subtree->second.begin(), subtree->second.end(), int(directoryId));
	if (subtreeDir == subtree->second.end())
	{
		qWarning("Missing directory: %d in subtree %d", directoryId, subtreeId);
		return odstranene;
	}
	subtree->second.erase(subtreeDir);

	return odstranene;
}

/*!
  \brief Vrati nazov adresara.

  \param directoryId ID adresara.

  V pripade najdenia adresara vrati jeho nazov, v opacnom pripade NULL.
*/
string *SNSimulate::directory(int directoryId)
{
	map<int, pair<int, string> >::iterator dir;
	dir = m_folders.find(-directoryId);
	if (dir == m_folders.end())
	{
		return NULL;
	}
	else
	{
		return &dir->second.second;
	}
}

/*!
  \fn SNSimulate::telnetResponseRecived(uint32_t id, const char *text, const char *cmd)

  Tento signal sa emituje po prijati telnet dat od zariadenia.
*/