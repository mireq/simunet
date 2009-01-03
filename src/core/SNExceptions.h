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
#ifndef SNEXCEPTIONS_H
#define SNEXCEPTIONS_H

#include <string>

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/

class SNDeviceImportException
{
	public:
		SNDeviceImportException(const std::string &a_moduleName);
		const std::string &moduleName();
	private:
		std::string m_moduleName;
};

class SNPythonInterpreterException
{
	public:
		enum pythonErr {CREATE, IMPORT, ATTR, CALL, SET};

		SNPythonInterpreterException(const std::string &a_problem, pythonErr a_type);
		const std::string &problem();
		pythonErr problemType();
	private:
		std::string m_problem;
		pythonErr m_type;
};

class PyObjectNULLException
{
	public:
		PyObjectNULLException();
};


#endif
