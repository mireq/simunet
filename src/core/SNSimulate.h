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
 @brief Trieda zabezpecujuca samotnu simulaciu.
*/
class SNSimulate
{
	public:
		SNSimulate(int threads);
		~SNSimulate();

/*!
 \brief Odstranenie zariadenia zo simulacie.
 \param id Unikatne identifikacne cislo zariadenia.
 \return Po uspesnom odstraneni je navratova hodnota false, pri chybe true.
*/
		bool stopDevice(uint32_t id);

/*!
 \brief Pridanie zariadenia do simulacie.
 \return Navratovou hodnotou je jedinecne identifikacne cislo zariadenia.
*/
		uint32_t startDevice(const std::string &filename);

/*!
 \brief Spracovanie ramca / preposlanie inym zariadeniam.
 \param id ID zariadenia.
 \param data Data ktore poslalo zariadenie.
 Tato metoda zachytava spravy od zariadeni a spracuje ich / preposiela
 ostatnym zariadeniam.
*/
		void processFrame(uint32_t id, PyObject *data);

/*!
 \brief Prijatie telnet odpovede od zariadenia.
 \param id ID zariadenia.
 \param text Text ktory vypisalo zariadenie.
 \param cmd Text ktory sa ma pouzit ako prompt.
*/
		void telnetResponse(uint32_t id, const char *text, const char *cmd);

	private:
		int m_threadCount;
		std::list<SNSimulateHelper *> m_simulateHelpers;
		std::list<SNSimulateHelper *>::iterator m_nextSimulateHelper;
		std::map<int, SNDevice*> m_devices;
		int m_nextDeviceId;
};


#endif
