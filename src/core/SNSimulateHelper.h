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
#ifndef SNSIMULATEHELPER_H
#define SNSIMULATEHELPER_H

#include <QThread>
#include <list>
#include <Python.h>
#include "sntypes.h"

class SNDevice;

/**
 @author Miroslav Bendik
*/
class SNSimulateHelper: public QThread
{
	Q_OBJECT
	public:
		SNSimulateHelper(PyThreadState *mainThreadState);
		~SNSimulateHelper();
		void run();
		void stop();
		void addDevice(SNDevice *device);
		void sendFrame(uint32_t targetDevId, port_num hwPort, PyObject *frame);
	private:
/*!
  Zoznam zariadeni prisluchajucich danemu vlaknu.
*/
		std::list<SNDevice *> m_devices;
/*!
  Odkaz na celkovy zoznam zairadeni.
*/
		PyObject *m_pDevicesDict;
/*!
  Odkaz na objekt SNSimulateHelper pouzivany vy pythonovi.
*/
		PyObject *m_simulateHelper;
/*!
  Odkaz na hlavne vlakno.
*/
		static PyThreadState *m_mainThreadState;
/*!
  Odkaz na vlakno tohto objektu.
*/
		PyThreadState *m_threadState;

		bool m_stop;

};

#endif
