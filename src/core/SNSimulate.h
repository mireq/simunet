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
		void frameResponse(uint32_t id, PyObject *data);

/*!
 \brief Prijatie telnet odpovede od zariadenia.
 \param id ID zariadenia.
 \param text Text ktory vypisalo zariadenie.
 \param cmd Text ktory sa ma pouzit ako prompt.
*/
		void telnetResponse(uint32_t id, const char *text, const char *cmd);

/*!
 \brief Ziskanie referencie na zariadenie
 \param id ID zariadenia
 \return Navratovou hodotou je referencia zariadenia ak existuje, NULL ak neexistuje.
*/
		SNDevice *device(uint32_t id) const;

		const std::vector<int> *devicesList(int parent = 0) const;
		int findIndexOfDevice(int devId, int parent = 0) const;
		int parent(int devId) const;
		void move(int devId, int row, int parent = 0);
		void removeFromSubtree(int devId, int parent = 0);
		void addToSubtree(int devId, int row, int parent = 0);
		void addDirectory(std::string name, int parent = 0);
		bool removeDirectory(int directoryId);
		std::string *directory(int directoryId);

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
		std::list<SNSimulateHelper *> m_simulateHelpers;

/*!
  \brief Iterator ukazujuci na nasledujuce vlakno
  Po nastartovani zariadenia sa posunie na dalise vlakno aby boli zariadenia
  pridelovane postupne jednotlivym vlaknam. Pri poslednom vlakne sa znovu
  nastavi na zaciatok zoznamu.
*/
		std::list<SNSimulateHelper *>::iterator m_nextSimulateHelper;

/*!
  \brief Pole zariadeni
  Asociativne pole ktoreho klucom je unikatne ID zariadenia.
  Hodnotou je dvojica: ID nadradenej polozky, ukazovatel na zariadenie.
  V pripade, ze ID nadradenej polozky je 0 znamena to, ze zariadenie je
  v korenovom adresari.
*/
		std::map<int, std::pair<int, SNDevice*> > m_devices;
		std::map<int, std::pair<int, std::string> > m_folders;

/*!
  \brief Stromova struktura s indexmi zariadeni
  Toto asociativne pole ma ako kluc ID virtualneho adresara kde je zariadenie
  umiestnene. Hodnotou je zoznam zariadeni (kladne cislo) a podadresarov
  (zaporne cislo v absolutnej hodnote reprezentuje ID adresara). Korenovy
  adresar ma cislo 0.
*/
		std::map<int, std::vector<int> > m_devicesTree;

/*!
  \brief ID nasledujuceho zariadenia
  Aby sme zabranili duplicitam pridelujeme kazdemu dalsiemu zariadeniu vzdy
  nasledujuce ID (aj keby sa niektore z predchadzajucich uvolnili).
*/
		int m_nextDeviceId;
		int m_nextFolderId;
		PyThreadState *m_mainThreadState;
};


#endif
