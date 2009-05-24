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

#include "SNExceptions.h"

using namespace std;

/*!
  \class SNDeviceImportException
  \brief Vynimka ktora sa vyvolava pri chybe importu zariadenia.
  \ingroup exceptions
*/

/*!
  Vytvorenie vynimky.

  \param moduleName Nazov zariadenia ktore sa nepodarilo nastartovat.
*/
SNDeviceImportException::SNDeviceImportException(const std::string &moduleName)
{
	m_moduleName = moduleName;
}

/*!
  Zistenie nazvu chybneho zariadenia.
*/
const std::string &SNDeviceImportException::moduleName()
{
	return m_moduleName;
}

/*!
  \class SNPythonInterpreterException
  \brief Vynimka sa vykonava pri chybe spoluprace s python intepreterom.
  \ingroup exceptions
*/

/*!
  Vytvorenie novej vynimke pre chybe interpretovania python kodu.

  \param problem Strucny popis problemu v textovej podobe.
  \param type Typ problemu ktory sa vyskytol.
*/
SNPythonInterpreterException::SNPythonInterpreterException(const std::string &problem, SNPythonInterpreterException::pythonErr type)
{
	m_problem = problem;
	m_type = type;
}

/*!
  Strucny popis problemu v textovej forme.
*/
const std::string & SNPythonInterpreterException::problem()
{
	return m_problem;
}

/*!
  Typ problemu ktory nastal.
*/
SNPythonInterpreterException::pythonErr SNPythonInterpreterException::problemType()
{
	return m_type;
}

/*!
  \class PyObjectNULLException
  \brief Vynimka vyvolana pri vratenie NULL namiesto PyObject *
  \ingroup exceptions
*/

/*!
  Vynimka ktora sa vyvola v pripade, ze python vrati namiesto PyObject * hodnotu
  NULL.
*/
PyObjectNULLException::PyObjectNULLException()
{
}

