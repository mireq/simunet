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

#include <iostream>

using namespace std;

const PyMethodDef SNSimulate::SNSimulateMethods[] = {
	{"sendFrame", SNSimulate::processFrameWrapper, METH_VARARGS, "Odoslanie ramca"},
	{"sendTelnet", SNSimulate::telnetResponseWrapper, METH_VARARGS, "Odoslanie dat cez telnet"},
	{NULL, NULL, 0, NULL}
};

/*!
    \fn SNSimulate::SNSimulate()
    Vytvorenie 4 vlakien SNSimulateHelper
*/
SNSimulate::SNSimulate()
{
	/// @todo zmenit napevno nastaveny pocet vlakien
	m_threadCount = 4;
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
	list<SNSimulateHelper *>::iterator helper;
	for (helper = m_simulateHelpers.begin(); helper != m_simulateHelpers.end(); ++helper)
	{
		(*helper)->stop();
	}
	for (helper = m_simulateHelpers.begin(); helper != m_simulateHelpers.end(); ++helper)
	{
		(*helper)->wait(1000);
		delete *helper;
	}
}


/*!
    \fn SNSimulate::stopDevice(uint32_t id)
 */
bool SNSimulate::stopDevice(uint32_t id)
{
    /// @todo implement me
}


/*!
    \fn SNSimulate::startDevice(const string &filename)
 */
uint32_t SNSimulate::startDevice(const string &filename)
{
	SNDevice *device = new SNDevice(filename, m_nextDeviceId);
	(*m_nextSimulateHelper)->addDevice(device);
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

PyObject *SNSimulate::processFrameWrapper(PyObject *self, PyObject *args)
{
	if (PyTuple_Size(args) != 2)
	{
		return NULL;
	}

	PyCPPObject pDeviceId(PyTuple_GetItem(args, 0));
	PyCPPObject pData(PyTuple_GetItem(args, 1));

	uint32_t deviceId = PyLong_AsUnsignedLong(pDeviceId);
	processFrame(deviceId, pData);

	Py_RETURN_NONE;
}

PyObject* SNSimulate::telnetResponseWrapper(PyObject * self, PyObject * args)
{
	if (PyTuple_Size(args) != 3)
	{
		return NULL;
	}

	PyCPPObject pDeviceId(PyTuple_GetItem(args, 0));
	PyCPPObject pText(PyTuple_GetItem(args, 1));
	PyCPPObject pCmd(PyTuple_GetItem(args, 2));

	uint32_t deviceId = PyLong_AsUnsignedLong(pDeviceId);
	if (!PyString_Check(pText) || !PyString_Check(pCmd))
	{
		return NULL;
	}

	char *text = PyString_AsString(pText);
	char *cmd  = PyString_AsString(pCmd);
	telnetResponse(deviceId, text, cmd);

	Py_RETURN_NONE;
}
