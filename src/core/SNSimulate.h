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
#include <vector>
#include <set>

#include "sntypes.h"
#include "SNHwPort.h"

#include <QObject>

class SNDevice;
class SNMap;
class SNSimulateHelper;

/**
 @author Miroslav Bendik
*/
class SNSimulate: public QObject
{
	Q_OBJECT
	public:
		SNSimulate(int threads);
		~SNSimulate();
		bool stopDevice(uint32_t id);
		uint32_t startDevice(const std::string &filename);
		void frameResponse(uint32_t id, port_num hwPort, PyObject *data);
		void telnetResponse(uint32_t id, const char *text, const char *cmd);

		// zistovanie informacii o zariadeniach pre model
		SNDevice *device(uint32_t id) const;

		// komunikacia so zariadeniami
		char *httpRequest(uint32_t devId, const std::string &url, PyObject *get, PyObject *post);
		char *httpRequest(uint32_t devId, const std::string &url, const std::map<std::string, std::string> get, const std::map<std::string, std::string> post);

		char *telnetRequest(uint32_t devId, const std::string &line, char symbol);
		void sendFrame(uint32_t targetDevId, port_num hwPort, PyObject *frame);

		void hwPortInserted(uint32_t devId, port_num hwPort);
		void hwPortRemoved(uint32_t devId, port_num hwPort);

		void setMap(SNMap *map);

		// vytvaranie a rusenie spojeni
		void addConnection(uint32_t dev1, port_num port1, uint32_t dev2, port_num port2);
		void removeConnection(uint32_t dev1, port_num port1, uint32_t dev2, port_num port2);

	signals:
		void telnetResponseRecived(uint32_t id, const char *text, const char *cmd);

	private:
		void setPath();
		void createDevicesDictionary();
		void createSNSimulateModule();
		void createBaseClass();

/*!
  \brief Pocet vlakien ktore budu spustene
 */
		int m_threadCount;

/*!
  \brief Zoznam vlakien
*/
		std::vector<SNSimulateHelper *> m_simulateHelpers;

/*!
  \brief Iterator ukazujuci na nasledujuce vlakno
  Po nastartovani zariadenia sa posunie na dalise vlakno aby boli zariadenia
  pridelovane postupne jednotlivym vlaknam. Pri poslednom vlakne sa znovu
  nastavi na zaciatok zoznamu.
*/
		std::vector<SNSimulateHelper *>::iterator m_nextSimulateHelper;

/*!
  \brief Pole zariadeni
  Asociativne pole ktoreho klucom je unikatne ID zariadenia.
  Hodnotou je ukazovatel na zariadenie.
*/
		std::map<uint32_t, SNDevice*> m_devices;


/*!
  \brief ID nasledujuceho zariadenia
  Aby sme zabranili duplicitam pridelujeme kazdemu dalsiemu zariadeniu vzdy
  nasledujuce ID (aj keby sa niektore z predchadzajucich uvolnili).
*/
		uint32_t m_nextDeviceId;

/*!
  \brief Zoznam portov zariadeni a ich susedov.
*/
		std::set<SNHwPort> m_connections;

/*!
  \brief Odkaz na hlavne vlakno.
*/
		PyThreadState *m_mainThreadState;

/*!
  \brief Odkaz na mapu, do ktorej sa premietaju zmeny.
*/
		SNMap *m_map;
};


#endif
