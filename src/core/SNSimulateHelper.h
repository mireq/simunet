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
		void sendFrame(uint32_t targetDevId, PyObject *frame);
	private:
		bool m_stop;
		std::list<SNDevice *> m_devices;
		PyObject *m_pDevicesDict;
		PyObject *m_simulateHelper;
		static PyThreadState *m_mainThreadState;
		PyThreadState *m_threadState;

};

#endif
