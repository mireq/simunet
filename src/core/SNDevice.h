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

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
 @brief Trieda prepajajuca svet pythonu s C++.
*/
class SNDevice
{
	public:
/*!
 \brief Vytvorenie zariadenia.
 \param filename Adresar so zariadenim (v rovnakom tvare ako pouziva python import).
*/
		SNDevice(const std::string &filename, uint32_t deviceId);
		~SNDevice();

/*!
 \brief Odoslanie ramca zariadeniu.
*/
		bool processFrame(PyObject *data);

/*!
 \brief Vymazanie konfiguracie zariadenia.
*/
		bool resetConfig(void);

/*!
 \brief Nastavenie konfiguracie zariadenia.
*/
		bool setConfig(PyObject *data);

/*!
 \brief Ziskanie konfiguracie zariadenia.
*/
		PyObject *dumpConfig(void);

/*!
 \brief Odoslanie http poziadavky zariadeniu.
 \param url Url ktore pozadujeme od http serveru zariadenia.
 \param post Post data ktore mu posielame.
*/
		char *httpRequest(const std::string &url, PyObject *post);

/*!
 \brief Odoslanie vstupneho riadku z telnetu a symbolu ktory prenos odstartoval.
 \param line Vstupny riadok.
 \param symbol Znak ktory odstartoval prenos (napr \\n, alebo ? na CISCO zariadeniach.)
*/
		char *telnetRequest(const std::string &line, char symbol);

/*!
 \brief Zistenie znakov po ktorych sa odosiela riadok.
*/
		char *telnetGetControlChars(void);

	private:
		uint32_t m_deviceId;
		PyObject *m_pDeviceInstance;
};

#endif
