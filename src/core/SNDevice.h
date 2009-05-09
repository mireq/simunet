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
#ifndef SNDEVICE_H
#define SNDEVICE_H

#include <Python.h>
#include <string>
#include <map>
#include <list>
#include <vector>

#include "sntypes.h"

class SNSimulate;

/**
 @author Miroslav Bendik
*/
class SNDevice
{
	public:
		SNDevice(const std::string &filename, uint32_t deviceId, SNSimulate *parent = 0);
		~SNDevice();

		uint32_t deviceId() const;

		bool processFrame(PyObject *data, port_num hwPort);
		bool resetConfig(void);
		bool setConfig(PyObject *data);
		PyObject *dumpConfig(void);
		char *httpRequest(const std::string &url, PyObject *post);
		char *httpRequest(const std::string &url, const std::map<std::string, std::string> post);
		char *telnetRequest(const std::string &line, char symbol);
		char *telnetGetControlChars(void);
		static const PyMethodDef SNSimulateMethods[];

		const std::vector<port_num> &portNumbers() const;

	private:
		uint32_t m_deviceId;
		PyObject *m_pDeviceInstance;
		SNSimulate *m_simulate;

		static PyObject *frameResponseWrapper(PyObject *self, PyObject *args);
		static PyObject *telnetResponseWrapper(PyObject *self, PyObject *args);

		static PyObject *insertHwPortWrapper(PyObject *self, PyObject *args);
		static PyObject *removeHwPortWrapper(PyObject *self, PyObject *args);
		static PyObject *setHwPortHandlerWrapper(PyObject *self, PyObject *args);

		port_num m_nextHwPortNum;
		std::vector<port_num> m_portNumbers;

};

#endif
