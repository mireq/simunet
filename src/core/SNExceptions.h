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

class SNException
{
};

/**
 @author Miroslav Bendik
*/
class SNDeviceImportException: public SNException
{
	public:
		SNDeviceImportException(const std::string &moduleName);
		const std::string &moduleName();
	private:
/*!
  Nazov modulu, ktory vyvolal vynimku.
*/
		std::string m_moduleName;
};

/**
 @author Miroslav Bendik
 */
class SNPythonInterpreterException: public SNException
{
	public:
/*!
  Typ chyby ktoru vracia interpreter pythonu.
*/
		enum pythonErr {
			CREATE, /*!< Chyba pri vytvarani objektov. */
			IMPORT, /*!< Chyba pri importe. */
			ATTR,   /*!< Chyba pri nastavovani alebo citani atributov. */
			CALL,   /*!< Chyba pri volani. */
			SET     /*!< Chyba pri nastavovani premennych. */
		};

		SNPythonInterpreterException(const std::string &problem, pythonErr type);
		const std::string &problem();
		pythonErr problemType();
	private:
/*!
  Problem v textovej podobe.
*/
		std::string m_problem;

/*!
  Typ chyby, ktora nastala.
*/
		pythonErr m_type;
};

/**
 @author Miroslav Bendik
 */
class PyObjectNULLException: public SNException
{
	public:
		PyObjectNULLException();
};

/**
 @author Miroslav Bendik
 \class SNoutOfRangeException
 \brief Vynimka vyvolana pri pokuse o pristp mimo hranic poli / zoznamov
 \ingroup exceptions
 */
class SNoutOfRangeException: public SNException
{
};


#endif
