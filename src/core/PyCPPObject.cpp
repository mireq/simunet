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
#include "PyCPPObject.h"
#include "SNExceptions.h"

PyCPPObject::PyCPPObject(PyObject *pyobject):m_keepRef(false)
{
	if (pyobject == NULL)
	{
		throw new PyObjectNULLException();
	}
	m_object = pyobject;
}


PyCPPObject::~PyCPPObject()
{
	if (m_object != NULL && m_keepRef == false)
	{
		Py_XDECREF(m_object);
	}
}


PyCPPObject::operator PyObject *()
{
	return m_object;
}

PyObject *PyCPPObject::operator ->()
{
	return m_object;
}

bool PyCPPObject::isClass()
{
	return PyClass_Check(m_object);
}

bool PyCPPObject::isCallable()
{
	return PyCallable_Check(m_object);
}

void PyCPPObject::keepRef()
{
	m_keepRef = true;
}


