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
#include "SNDevice.h"
#include "SNExceptions.h"
#include "PyCPPObject.h"

#include <iostream>
#include <algorithm>

#include <QDebug>

using namespace std;

/*!
  Metody ktore exportujeme do pythonu
*/
const PyMethodDef SNDevice::SNSimulateMethods[] = {
	{"sendFrame", SNDevice::frameResponseWrapper, METH_VARARGS, "Odoslanie ramca"},
	{"sendTelnet", SNDevice::telnetResponseWrapper, METH_VARARGS, "Odoslanie dat cez telnet"},
	{"insertHwPort", SNDevice::insertHwPortWrapper, METH_VARARGS, "Pridanie konektoru"},
	{"removeHwPort", SNDevice::removeHwPortWrapper, METH_VARARGS, "Odstranenie konektoru"},
	{"setHwPortHandler", SNDevice::setHwPortHandlerWrapper, METH_VARARGS, "Nastavenie obsluhy po prijati ramca na dany konektor"},
	{NULL, NULL, 0, NULL}
};

/*!
  \class SNDevice
  \brief Trieda prepajajuca svet pythonu s C++.
  \ingroup core

  Tato trieda zjednodusuje volanie python funkcii z C++.
*/

/*!
  Vytvorenie zariadenia.

  \param filename Adresar so zariadenim (v rovnakom tvare ako pouziva python import).
  \param deviceId Jednoznacny identifikator zariadenia.
  \param parent Odkaz na objekt typu SNSimulate
 */
SNDevice::SNDevice(const std::string &filename, uint32_t deviceId, SNSimulate *parent)
	: m_nextHwPortNum(1)
{
	m_simulate = parent;
	try
	{
		// import modulu, modul je vzdy v adresari s rovnakym nazvom preto modu.modul
		PyCPPObject pModuleName(PyString_FromString(string(filename + "." + filename).c_str()), true);
		PyCPPObject pModule(PyImport_Import(pModuleName), true);

		// natiahneme triedu reprezentujucu zariadenie
		PyCPPObject pDeviceClass(PyObject_GetAttrString(pModule, filename.c_str()));

		// odtestujem ci sme skutocne nacitali triedu
		if (!pDeviceClass.isClass())
		{
			throw SNDeviceImportException(filename);
		}

		// natiahneme globalnu premennu devices
		PyCPPObject pMainModule(PyImport_AddModule("snsimulate"));
		PyCPPObject pDevicesDict(PyObject_GetAttrString(pMainModule, "devices"), true);

		// vytvorime novu instanciu zariadenia
		//PyCPPObject pDeviceInstance(PyInstance_New(pDeviceClass, NULL, NULL), true);
		PyCPPObject pDeviceInstance(PyInstance_NewRaw(pDeviceClass, NULL), true);
		m_pDeviceInstance = pDeviceInstance;
		PyCPPObject pDeviceId(PyLong_FromUnsignedLong(deviceId), true);

		// a ulozime ju do asociativneho pola
		PyDict_SetItem(pDevicesDict, pDeviceId, pDeviceInstance);
		PyObject_SetAttrString(pDeviceInstance, "deviceId", pDeviceId);
		PyCPPObject pSNDevice(PyCObject_FromVoidPtr((void *)this, NULL), true);
		PyObject_SetAttrString(pDeviceInstance, "pSNDevice", pSNDevice);

		PyCPPObject constructor(PyObject_GetAttrString(pDeviceInstance, "__init__"), true);
		PyCPPObject args(PyTuple_New(0), true);
		PyObject_Call(constructor, args, NULL);
	}
	catch (PyObjectNULLException e)
	{
		throw SNDeviceImportException(filename);
	}
	m_deviceId = deviceId;
	std::cout<<std::flush;
}

/*!
  Zrusenie zariadenia
*/
SNDevice::~SNDevice()
{
	PyCPPObject pDeviceId(PyLong_FromUnsignedLong(m_deviceId), true);

	PyCPPObject pMainModule(PyImport_AddModule("snsimulate"));
	PyCPPObject pDevicesDict(PyObject_GetAttrString(pMainModule, "devices"), true);
	if (PyDict_Contains(pDevicesDict, pDeviceId) == 1)
	{
		PyCPPObject pDevice(PyDict_GetItem(pDevicesDict, pDeviceId));
		try
		{
			PyCPPObject pStopFunc(PyObject_GetAttrString(pDevice, "stop"), true);
			if (pStopFunc.isCallable())
			{
				PyCPPObject args(PyTuple_New(0), true);
				Py_XDECREF(PyObject_Call(pStopFunc, args, NULL));
			}
		}
		catch (PyObjectNULLException e)
		{
		}
		PyDict_DelItem(pDevicesDict, pDeviceId);
	}
	std::cout<<std::flush;
}

/*!
  Získanie ID zariadenia.
*/
uint32_t SNDevice::deviceId() const
{
	return m_deviceId;
}


/*!
  Odoslanie ramca zariadeniu. Argument \a port_num urcuje port fyzicky port
  na ktory sa ma ramec odoslat.
 */
bool SNDevice::processFrame(PyObject *data, port_num hwPort)
{
	PyCPPObject pProcessFrameFunc(PyObject_GetAttrString(m_pDeviceInstance, "processFrame"), true);
	if (!pProcessFrameFunc.isCallable())
	{
		return true;
	}
	PyCPPObject port(PyLong_FromUnsignedLong(hwPort));
	PyCPPObject args(PyTuple_New(2), true);
	Py_INCREF(data);
	PyTuple_SetItem(args, 0, port);
	PyTuple_SetItem(args, 1, data);
	PyCPPObject ret(PyObject_Call(pProcessFrameFunc, args, NULL));
	if (!PyBool_Check(ret))
	{
		return true;
	}
	else
	{
		return PyObject_IsTrue(ret);
	}
}


/*!
  Vymazanie konfiguracie zariadenia.
 */
bool SNDevice::resetConfig(void)
{
	try
	{
		PyCPPObject resetConfigFunc(PyObject_GetAttrString(m_pDeviceInstance, "resetConfig"), true);
		if (!resetConfigFunc.isCallable())
		{
			return true;
		}
		PyCPPObject resetArgs(PyTuple_New(0), true);
		PyCPPObject ret(PyObject_Call(resetConfigFunc, resetArgs, NULL));
		return (PyObject_IsTrue(ret) == 1);
	}
	catch (PyObjectNULLException e)
	{
		return true;
	}
}


/*!
  Nastavenie konfiguracie zariadenia.
 */
bool SNDevice::setConfig(PyObject *data)
{
	try
	{
		PyCPPObject setConfigFunc(PyObject_GetAttrString(m_pDeviceInstance, "setConfig"), true);
		if (!setConfigFunc.isCallable())
		{
			return true;
		}
		PyCPPObject args(PyTuple_New(1), true);
		PyTuple_SetItem(args, 0, data);
		PyCPPObject ret(PyObject_Call(setConfigFunc, args, NULL));
		return (PyObject_IsTrue(ret) == 1);
	}
	catch (PyObjectNULLException e)
	{
		return true;
	}
}

/*!
  Ziskanie konfiguracie zariadenia.
 */
PyObject *SNDevice::dumpConfig(void)
{
	try
	{
		PyCPPObject dumpConfigFunc(PyObject_GetAttrString(m_pDeviceInstance, "dumpConfig"), true);
		if (!dumpConfigFunc.isCallable())
		{
			return NULL;
		}
		PyCPPObject dumpArgs(PyTuple_New(0), true);
		return PyObject_Call(dumpConfigFunc, dumpArgs, NULL);
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}


/*!
 Odoslanie http poziadavky zariadeniu.

 \param url Url ktore pozadujeme od http serveru zariadenia.
 \param get Get data, ktore posielame zariadeniu.
 \param post Post data ktore mu posielame.
 */
char *SNDevice::httpRequest(const std::string &url, PyObject *get, PyObject *post)
{
	try
	{
		PyCPPObject httpRequestFunc(PyObject_GetAttrString(m_pDeviceInstance, "httpRequest"), 2);
		if (!httpRequestFunc.isCallable())
		{
			return NULL;
		}
		PyCPPObject pUrl(PyString_FromString(url.c_str()));
		PyCPPObject args(PyTuple_New(3), true);

		PyTuple_SetItem(args, 0, pUrl);
		PyTuple_SetItem(args, 1, get);
		PyTuple_SetItem(args, 2, post);
		PyCPPObject pRet(PyObject_CallObject(httpRequestFunc, args));
		//return PyString_AsString(pRet);
		if (!PyString_Check(pRet))
		{
			return NULL;
		}
		else
		{
			return PyString_AsString(pRet);
		}
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}

/*!
  \overload
*/
char *SNDevice::httpRequest(const std::string &url, const std::map<std::string, std::string> get, const std::map<std::string, std::string> post)
{
	try
	{
		PyCPPObject pGet(PyDict_New(), true);
		PyCPPObject pPost(PyDict_New(), true);
		map<string,string>::const_iterator iter;
		for (iter = get.begin(); iter != get.end(); ++iter)
		{
			PyDict_SetItemString(pGet, iter->first.c_str(), PyCPPObject(PyString_FromString(iter->second.c_str()), true));
		}
		for (iter = post.begin(); iter != post.end(); ++iter)
		{
			PyDict_SetItemString(pPost, iter->first.c_str(), PyCPPObject(PyString_FromString(iter->second.c_str()), true));
		}
		return httpRequest(url, pGet, pPost);
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}


/*!
  Odoslanie vstupneho riadku z telnetu a symbolu ktory prenos odstartoval.

  Pomocou tejto metody sa posilaju telnet data pre konkretne zariadenie.
  Tato funkcia vracia string ktory sa po spracovani requestu vypise na obrazovku.

  \param line Vstupny riadok.
  \param symbol Znak ktory odstartoval prenos (napr \\n, alebo ? na CISCO zariadeniach.)
 */
char *SNDevice::telnetRequest(const std::string &line, char symbol)
{
	try
	{
		PyCPPObject telnetRequestFunc(PyObject_GetAttrString(m_pDeviceInstance, "telnetRequest"), true);
		if (!telnetRequestFunc.isCallable())
		{
			return NULL;
		}
		PyCPPObject args(PyTuple_New(2), true);

		PyCPPObject pLine(PyString_FromString(line.c_str()));
		PyCPPObject pSymbol(PyString_FromFormat("%c", symbol));

		PyTuple_SetItem(args, 0, pLine);
		PyTuple_SetItem(args, 1, pSymbol);

		PyCPPObject pRet(PyObject_CallObject(telnetRequestFunc, args));
		if (!PyString_Check(pRet))
		{
			return NULL;
		}
		else
		{
			return PyString_AsString(pRet);
		}
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}


/*!
  Ziskanie zoznamu znakov, po ktorych odosielame data zariadeniu.
 */
char *SNDevice::telnetGetControlChars(void)
{
	try
	{
		PyCPPObject telnetGetControlCharsFunc(PyObject_GetAttrString(m_pDeviceInstance, "telnetGetControlChars"), true);
		if (!telnetGetControlCharsFunc.isCallable())
		{
			return NULL;
		}
		PyCPPObject args(PyTuple_New(0), true);
		PyCPPObject pRet(PyObject_Call(telnetGetControlCharsFunc, args, NULL));
		return PyString_AsString(pRet);
	}
	catch (PyObjectNULLException e)
	{
		return NULL;
	}
}

/*!
  Ziskanie zoznamu portov, ktore ma zariadenie.
*/
const std::vector<port_num> &SNDevice::portNumbers() const
{
	return m_portNumbers;
}

/*!
  Reakcia na prijatie ramca odoslaneho zariadenim.
*/
PyObject *SNDevice::frameResponseWrapper(PyObject *, PyObject *args)
{
	if (PyTuple_Size(args) != 4)
	{
		return NULL;
	}

	PyCPPObject pSNDeviceInstance(PyTuple_GetItem(args, 0));
	PyCPPObject pDeviceId(PyTuple_GetItem(args, 1));
	PyCPPObject pPortId(PyTuple_GetItem(args, 2));
	PyCPPObject pData(PyTuple_GetItem(args, 3));

	uint32_t deviceId = PyLong_AsUnsignedLong(pDeviceId);
	port_num hwPort = PyLong_AsUnsignedLong(pPortId);
	SNDevice *dev = (SNDevice *)PyCObject_AsVoidPtr(pSNDeviceInstance);
	if (dev->m_simulate != NULL)
	{
		dev->m_simulate->frameResponse(deviceId, hwPort, pData);
	}

	Py_RETURN_NONE;
}

/*!
  Spracovanie telnet dat prijatych od zariadenia.
*/
PyObject* SNDevice::telnetResponseWrapper(PyObject *, PyObject *args)
{
	if (PyTuple_Size(args) != 4)
	{
		return NULL;
	}

	PyCPPObject pSNDeviceInstance(PyTuple_GetItem(args, 0));
	PyCPPObject pDeviceId(PyTuple_GetItem(args, 1));
	PyCPPObject pText(PyTuple_GetItem(args, 2));
	PyCPPObject pCmd(PyTuple_GetItem(args, 3));

	uint32_t deviceId = PyLong_AsUnsignedLong(pDeviceId);
	if (!PyString_Check(pText) || !PyString_Check(pCmd))
	{
		return NULL;
	}

	char *text = PyString_AsString(pText);
	char *cmd  = PyString_AsString(pCmd);
	SNDevice *dev = (SNDevice *)PyCObject_AsVoidPtr(pSNDeviceInstance);
	if (dev->m_simulate != NULL)
	{
		dev->m_simulate->telnetResponse(deviceId, text, cmd);
	}

	Py_RETURN_NONE;
}

/*!
  Spracovanie poziadavky zariadenia na pridanie fyzickeho portu.
*/
PyObject *SNDevice::insertHwPortWrapper(PyObject *, PyObject *args)
{

	if (PyTuple_Size(args) != 1)
	{
		return NULL;
	}

	PyCPPObject pSNDeviceInstance(PyTuple_GetItem(args, 0));
	/*PyCPPObject pHwPortPosition(PyTuple_GetItem(args, 1));
	if (!PyInt_Check(pHwPortPosition))
	{
		Py_RETURN_FALSE;
	}*/
	SNDevice *dev = (SNDevice *)PyCObject_AsVoidPtr(pSNDeviceInstance);

	/*list<port_num>::size_type position = PyInt_AsSsize_t(pHwPortPosition);
	if (position > dev->m_portNumbers.size())
	{
		Py_RETURN_FALSE;
	}

	dev->m_portNumbers.insert(dev->m_portNumbers.begin() + position, dev->m_nextHwPortNum);*/
	dev->m_portNumbers.push_back(dev->m_nextHwPortNum);
	if (dev->m_simulate != NULL)
	{
		dev->m_simulate->hwPortInserted(dev->m_deviceId, dev->m_nextHwPortNum);
	}

	dev->m_nextHwPortNum++;

	Py_RETURN_TRUE;
}

/*!
  Spracovanie poziadavky zariadenia na odstranenie fyzickeho portu.
*/
PyObject *SNDevice::removeHwPortWrapper(PyObject *, PyObject *args)
{
	if (PyTuple_Size(args) != 2)
	{
		return NULL;
	}

	PyCPPObject pSNDeviceInstance(PyTuple_GetItem(args, 0));
	PyCPPObject pPortNum(PyTuple_GetItem(args, 1));
	SNDevice *dev = (SNDevice *)PyCObject_AsVoidPtr(pSNDeviceInstance);

	if (!PyInt_Check(pPortNum))
	{
		Py_RETURN_FALSE;
	}
	port_num portNum = PyInt_AsLong(pPortNum);

	vector<port_num>::iterator portIter;
	portIter = find(dev->m_portNumbers.begin(), dev->m_portNumbers.end(), portNum);
	if (portIter == dev->m_portNumbers.end())
	{
		Py_RETURN_FALSE;
	}

	if (dev->m_simulate != NULL)
	{
		dev->m_simulate->hwPortRemoved(dev->m_deviceId, portNum);
	}
	dev->m_portNumbers.erase(portIter);

	Py_RETURN_TRUE;
	
}

/*!
  Nastavenie obsluhy fyzickeho portu zariadenia.
*/
PyObject *SNDevice::setHwPortHandlerWrapper(PyObject *, PyObject *)
{
	Py_RETURN_NONE;
}

