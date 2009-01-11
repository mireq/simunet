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

#ifndef SNSIMULATE_H
#define SNSIMULATE_H

#include <Python.h>
#include <string>
#include <list>
#include <map>

class SNDevice;
class SNSimulateHelper;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/
class SNSimulate
{
	public:
		SNSimulate();

		~SNSimulate();
		bool stopDevice(uint32_t id);
		uint32_t startDevice(const std::string &filename);
		static void processFrame(uint32_t id, PyObject *data);
		static void telnetResponse(uint32_t id, const char *text, const char *cmd);
		static const PyMethodDef SNSimulateMethods[];

	private:
		static PyObject *processFrameWrapper(PyObject *self, PyObject *args);
		static PyObject *telnetResponseWrapper(PyObject *self, PyObject *args);
		int m_threadCount;
		std::list<SNSimulateHelper *> m_simulateHelpers;
		std::list<SNSimulateHelper *>::iterator m_nextSimulateHelper;
		std::map<int, SNDevice*> m_devices;
		int m_nextDeviceId;
};


#endif
