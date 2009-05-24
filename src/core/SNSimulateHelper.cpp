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

#include <QDebug>

PyThreadState *SNSimulateHelper::m_mainThreadState = NULL;

/*!
  \class SNSimulateHelper
  \brief Trieda zabezpecujuca beh jednotlivych modulov.
  \ingroup core

  Kazda instancia tejto triedy sa stara o jedno vlakno v pythonovi.
*/


/*!
  Vytvorenie novej instancie a noveho vlakna v pythonovi.
*/
SNSimulateHelper::SNSimulateHelper(PyThreadState *mainThreadState)
	:m_stop(false)
{
	if (m_mainThreadState == NULL)
	{
		m_mainThreadState = mainThreadState;
	}

	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);
	PyCPPObject pSimuNetModule(PyImport_AddModule("SimuNet"));
	PyCPPObject pSimulateHelperClass(PyObject_GetAttrString(pSimuNetModule, "SimulateHelper"));
	m_simulateHelper = PyInstance_New(pSimulateHelperClass, NULL, NULL);
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
}

/*!
  Zrusenie instancie, zastavenie cinnosti vlakna.
*/
SNSimulateHelper::~SNSimulateHelper()
{
	PyEval_AcquireLock();
	Py_XDECREF(m_simulateHelper);
	PyEval_ReleaseLock();
}

/*!
  Spustenie simulacie (v samostatnom vlakne).
 */
void SNSimulateHelper::run()
{
	while(1)
	{
		//std::cout<<"Vlakno"<<std::endl;
		/*PyEval_AcquireLock();
		PyThreadState_Swap(m_threadState);
		PyThreadState_Swap(m_mainThreadState);
		PyEval_ReleaseLock();*/
		if (m_stop)
		{
			return;
		}
		sleep(1);
	}
}

/*!
  Zastavenie vlakna.

  Vlakno sa zastavi s istym oneskorenim, je potrebne pouzit join.
 */
void SNSimulateHelper::stop()
{
	sendFrame(0, 0, Py_None);
	m_stop = true;
}

/*!
  Pridanie zariadenia ktore sa bude v tomto vlakne spracovavat.
 */
void SNSimulateHelper::addDevice(SNDevice *device)
{
	m_devices.push_back(device);
}

/*!
  Odoslanie ramca zariadeniu urcenemu jeho ID - \a targetDevId na port \a hwPort.
  Data su v strukture \a frame.
*/
void SNSimulateHelper::sendFrame(uint32_t targetDevId, port_num hwPort, PyObject *frame)
{
	PyEval_AcquireLock();
	PyThreadState_Swap(m_mainThreadState);

	PyCPPObject pPutJob(PyObject_GetAttrString(m_simulateHelper, "put"));
	PyCPPObject putArgs(PyTuple_New(3), true);

	PyCPPObject pTargetDevId(PyLong_FromUnsignedLong(targetDevId));
	PyCPPObject pPort(PyLong_FromUnsignedLong(hwPort));

	PyTuple_SetItem(putArgs, 0, pTargetDevId);
	PyTuple_SetItem(putArgs, 1, pPort);
	PyTuple_SetItem(putArgs, 2, frame);

	PyCPPObject ret(PyObject_Call(pPutJob, putArgs, NULL));

	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
}

