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

PyThreadState *SNSimulateHelper::m_mainThreadState = NULL;

SNSimulateHelper::SNSimulateHelper()
	:m_stop(false)
{
	if (m_mainThreadState == NULL)
	{
		m_mainThreadState = PyThreadState_Get();
		PyEval_ReleaseLock();
	}
	PyEval_AcquireLock();
	PyInterpreterState *mainInterpreterState = m_mainThreadState->interp;
	m_threadState = PyThreadState_New(mainInterpreterState);
	PyEval_ReleaseLock();
}


SNSimulateHelper::~SNSimulateHelper()
{
	PyEval_AcquireLock();
	PyThreadState_Swap(NULL);
	PyThreadState_Clear(m_threadState);
	PyThreadState_Delete(m_threadState);
	PyThreadState_Swap(m_mainThreadState);
	PyEval_ReleaseLock();
}

void SNSimulateHelper::run()
{
	while(1)
	{
		//std::cout<<"Vlakno"<<std::endl;
		PyEval_AcquireLock();
		PyThreadState_Swap(m_threadState);
		PyThreadState_Swap(m_mainThreadState);
		PyEval_ReleaseLock();
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

