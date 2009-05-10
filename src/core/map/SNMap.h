/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2009 by Miroslav Bendik                                 *
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
#ifndef SNMAP_H
#define SNMAP_H

#include <string>
#include <map>
#include <list>
#include <set>

#include "sntypes.h"

class SNSimulate;
class SNDevTreeItem;
class SNDevTreeNode;
class SNDevice;
class SNAbstractDevicesScene;
class SNMapItem;
class SNMapDeviceItem;


/**
 @author Miroslav Bendik
*/
class SNMap
{
	public:
		SNMap();
		~SNMap();

		bool stopDevice(uint32_t devId);
		uint32_t startDevice(const std::string &filename);

		void insertDevice(uint32_t devId, uint32_t parent, int pos);
		void addDirectory(std::string name, uint32_t parent, int pos);

		int numItems(uint32_t parent) const;

		SNDevTreeItem *itemAt(int pos, uint32_t parent = 0) const;
		SNDevTreeItem *item(uint32_t internalId) const;
		SNMapDeviceItem *mapDeviceItem(uint32_t devId);

		int itemIndex(uint32_t internalId, uint32_t parent = 0) const;

		void deleteItem(uint32_t internalId, uint32_t parent = 0);

		void removeItem(uint32_t internalId, uint32_t parent = 0);
		void insertItem(uint32_t internalId, uint32_t parent, int pos);

		std::list<SNDevTreeItem *> itemsInTree(uint32_t parent) const;

		SNDevice *device(uint32_t devId);

		void setScene(SNAbstractDevicesScene *scene);
		SNAbstractDevicesScene *scene() const;

		// porty
		void insertPort(uint32_t devId, port_num hwPort);
		void removePort(uint32_t devId, port_num hwPort);

		void updateDevice(SNMapDeviceItem *device);
	private:
/*!
  Odkaz na simulator.
*/
		SNSimulate *m_simulate;

/*!
  Zoznam uzlov je asociativne pole. Klucom je cislo nadradeneho adresara.
  Korenovy uzol ma cislo nadradeneho adresara 0.
*/
		std::map<uint32_t, SNDevTreeNode*> m_itemsTree;

/*!
  Zoznam poloziek pricom klucom je ID polozky, hodnotou polozka samotna.
*/
		std::map<uint32_t, SNDevTreeItem*> m_items;

/*!
  Scena do ktorej sa premietaju zmeny v modeli.
*/
		SNAbstractDevicesScene *m_scene;

/*!
  Zoznam poloziek na mape.
*/
		std::set<SNMapItem *> m_mapItems;

/*!
  Zoznam zariadeni podla cisla zaraidenia.
*/
		std::map<uint32_t, SNMapDeviceItem *> m_devices;

};

#endif
