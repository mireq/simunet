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

#include <iostream>

/*!
  \class PyCPPObject
  \brief Reprezentacia PyObject * v C++
  \ingroup core

  Tato trieda sluzi na zjednodusenie prace s PyObject * v C++.
 */

/*!
  Vytvorenie novej instancie PyCPPObject zo struktury \a pyobject. Premenna
  \a clean rozhoduje o tom, ci sa ma tento objekt automaticky upratat pri
  uvolneni objekty PyCPPObject.
*/
PyCPPObject::PyCPPObject(PyObject *pyobject, bool clean)
{
	if (pyobject == NULL)
	{
		PyErr_Print();
		throw PyObjectNULLException();
	}
	m_clean = clean;
	m_object = pyobject;
}

/*!
  Zrusenie objektu PyCPPOjbect. Ak bol konstruktor volany s argumentom \a clean
  rovnym \e true potom bude znizeny pocet referencii na PyObject *.
*/
PyCPPObject::~PyCPPObject()
{
	
	if (m_object != NULL && m_clean == true)
	{
		Py_XDECREF(m_object);
	}
}

/*!
  Pretazeny operator dereferencie kvoli kompatibilite s PyObject *.
*/
PyCPPObject::operator PyObject *()
{
	return m_object;
}

/*!
  Pretazeny operator pristupu k zlozkam strukuty PyObject * kvoli kompatibilite
  s PyObject *.
*/
PyObject *PyCPPObject::operator ->()
{
	return m_object;
}

/*!
  Vrati \e true, ak je tento objekt triedou.
*/
bool PyCPPObject::isClass()
{
	return PyClass_Check(m_object);
}

/*!
  Vrati \e true, ak sa da tento objekt volat.
*/
bool PyCPPObject::isCallable()
{
	return PyCallable_Check(m_object);
}




