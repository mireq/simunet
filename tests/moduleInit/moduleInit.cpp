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


#include "core/SNDevice.h"
#include "core/PyCPPObject.h"
#include "shared.h"
#include <Python.h>

using namespace std;

int main(int argc, char *argv[])
{
	Py_Initialize();
	PyRun_SimpleString("import gc");
	PyRun_SimpleString("gc.set_debug(gc.DEBUG_LEAK)");
	interpreterInit();

	PyCPPObject pSimuNetModule(PyImport_AddModule("SimuNet"));
	PyCPPObject pSimuNetDict(PyModule_GetDict(pSimuNetModule));

	PyRun_String("class SNDevice:\n\tdef sendFrame(self, data):\t\tpass\n\tdef stop(self):\n\t\tpass\n", Py_single_input, pSimuNetDict, pSimuNetDict);
	for (int i = 1; i < argc; ++i)
	{
		SNDevice test(argv[i], i);
		PyCPPObject frame1(PyString_FromString("frame1"));
		test.processFrame(frame1, 0);
		PyCPPObject frame2(PyString_FromString("frame2"));
		test.processFrame(frame2, 0);
	}
//	Py_XDECREF(pDevicesDict);
// 	PyRun_SimpleString("gc.collect()");
// 	PyRun_SimpleString("print gc.garbage");
// 	PyRun_SimpleString("print gc.get_threshold()");
	Py_Finalize();
}
