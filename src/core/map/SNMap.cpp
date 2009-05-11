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
#include "SNMap.h"

#include "SNAbstractDevicesScene.h"
#include "SNDevTreeItem.h"
#include "SNDevTreeNode.h"
#include "SNMapDeviceItem.h"
#include "SNMapLineItem.h"


#include "core/SNAccessors.h"
#include "core/SNConfig.h"
#include "core/SNSimulate.h"

#include <vector>

#include <QDebug>

using namespace std;

/*!
  \class SNMap
  \brief Mapa obsahujuca jednotlive zariadenia.
  \ingroup map

  Tato trieda ma informacie o jednotlivych zariadeniach, ich ulozeni vo virtualnej
  adresarovej strukture. Okrem toho sa stara o prepojenia medzi zariadeniami,
  ma informacie o vsetkych kontrolnych bodoch cez ktore prechadzaju vodice.

  \sa SNSimulate
 */

/*!
  Vytvorenie novej mapy.

  \warning Tato trieda by sa mala vytvarat cez SNSingleton.

  \sa SNSingleton
*/
SNMap::SNMap()
	: m_scene(NULL)
{
	SNConfig config;
	m_simulate = SNSingleton::getSimulate();
	m_simulate->setMap(this);
}

/*!
  Zrusenie mapy.
*/
SNMap::~SNMap()
{
}

/*!
  Zrusenie zariadenia zo simulacie.

  \warning Tato funkcia nezrusi zariadenie zo zoznamov zariadeni preto je nutne
  pred volanim tento funkcie zavolat deleteItem.

  \sa SNSimulate::stopDevice, deleteItem
*/
bool SNMap::stopDevice(uint32_t devId)
{
	return m_simulate->stopDevice(devId);
}

/*!
  Spustenie noveho zariadenia.

  \warning Tato funkcia nepridava zariadenie do ziadneho zoznamu, je nutne ho
  tam pridat rucne volanim insertItem, alebo insertDevice.

  \sa SNSimulate::startDevice, insertItem, insertDevice
*/
uint32_t SNMap::startDevice(const std::string &filename)
{
	return m_simulate->startDevice(filename);
}

/*!
  Vlozenie zariadenia s identifikacnym cislom \a devId do adresara urceneho
  argumentom \a parent. Argument \a pos urcuje riadok do ktoreho sa vlozi
  zariadenie.
*/
void SNMap::insertDevice(uint32_t devId, uint32_t parent, int pos)
{
	SNMapDeviceItem *device = new SNMapDeviceItem(devId);
	m_mapItems.insert(device);
	m_devices[devId] = device;

	SNDevTreeDeviceItem *item = new SNDevTreeDeviceItem(device);
	map<uint32_t, SNDevTreeNode*>::iterator it;
	if (m_itemsTree.find(parent) == m_itemsTree.end())
	{
		m_itemsTree[parent] = new SNDevTreeNode(m_items[parent]);
	}
	m_itemsTree[parent]->insert(item, pos);
	m_items[item->internalId()] = item;

	// vlozenie zariadenia do sceny
	m_scene->addDevice(device);
}

/*!
  Vytvorenie noveho adresara s menom \a name v adresari \a parent. Ak je
  nadradeny adresar \a parent rovny 0 prida sa adresar do korenoveho adresara.
  Poradie v ktorom sa vlozi adresar je urceny argumentom \a pos.
*/
void SNMap::addDirectory(std::string name, uint32_t parent, int pos)
{
	SNDevTreeDirectoryItem *item = new SNDevTreeDirectoryItem(name);
	map<uint32_t, SNDevTreeNode*>::iterator it;
	if (m_itemsTree.find(parent) == m_itemsTree.end())
	{
		if (parent == 0)
		{
			m_itemsTree[parent] = new SNDevTreeNode(NULL);
		}
		else
		{
			m_itemsTree[parent] = new SNDevTreeNode(m_items[parent]);
		}
	}
	m_itemsTree[parent]->insert(item, pos);
	m_items[item->internalId()] = item;
}

/*!
  Vrati pocet poloziek v adresari urcenom paremetrom \a parent.
*/
int SNMap::numItems(uint32_t parent) const
{
	map<uint32_t, SNDevTreeNode*>::const_iterator it;
	it = m_itemsTree.find(parent);
	if (it == m_itemsTree.end())
	{
		return 0;
	}
	else
	{
		return it->second->childCount();
	}
}

/*!
  Vrati polozku stromu zaraideni, ktora sa nachadza v riadku urcenom argumentom
  \a pos nachadzajucu sa v adresari urcenom argumentom \a parent.
*/
SNDevTreeItem *SNMap::itemAt(int pos, uint32_t parent) const
{
	map<uint32_t, SNDevTreeNode*>::const_iterator it;
	it = m_itemsTree.find(parent);
	if (it == m_itemsTree.end())
	{
		return NULL;
	}
	else
	{
		return it->second->itemAt(pos);
	}

}

/*!
  Vrati polozku stromu zariadeni s internym ID \a intenralId.
*/
SNDevTreeItem *SNMap::item(uint32_t internalId) const
{
	map<uint32_t, SNDevTreeItem *>::const_iterator it;
	it = m_items.find(internalId);
	if (it == m_items.end())
	{
		return NULL;
	}
	else
	{
		return it->second;
	}
}

/*!
  Vrati zariadenie na mape podla ID zariadenia
*/
SNMapDeviceItem *SNMap::mapDeviceItem(uint32_t devId)
{
	std::map<uint32_t, SNMapDeviceItem *>::iterator device = m_devices.find(devId);
	if (device == m_devices.end())
	{
		return NULL;
	}
	else
	{
		return device->second;
	}
}

/*!
  Vrati riadok polozky s internym cislom \a internalId v adresari \a parent.
*/
int SNMap::itemIndex(uint32_t internalId, uint32_t parent) const
{
	
	map<uint32_t, SNDevTreeNode *>::const_iterator it;
	it = m_itemsTree.find(parent);
	if (it == m_itemsTree.end())
	{
		return -1;
	}
	return it->second->index(internalId);
}

/*!
  Odstranenie polozky alebo celeho adresara s internym ID \a internalId
  nachadzajucej sa v adresari \a parent.
*/
void SNMap::deleteItem(uint32_t internalId, uint32_t parent)
{
	// najdenie stromu
	map<uint32_t, SNDevTreeNode *>::iterator tree;
	tree = m_itemsTree.find(parent);
	if (tree == m_itemsTree.end())
	{
		return;
	}

	map<uint32_t, SNDevTreeItem *>::iterator itemIter;
	itemIter = m_items.find(internalId);
	if (itemIter != m_items.end())
	{
		SNDevTreeItem *it = itemIter->second;

		// odstranenie adresara
		map<uint32_t, SNDevTreeNode *>::iterator subtree;
		subtree = m_itemsTree.find(internalId);
		if (subtree != m_itemsTree.end())
		{
			vector<SNDevTreeItem *> *childs = subtree->second->childs();
			vector<SNDevTreeItem *>::iterator childIt;
			list<SNDevTreeItem *> itemsToDelete;
			for (childIt = childs->begin(); childIt != childs->end(); ++childIt)
			{
				itemsToDelete.push_back(*childIt);
			}
			list<SNDevTreeItem *>::iterator iter;
			for (iter = itemsToDelete.begin(); iter != itemsToDelete.end(); ++iter)
			{
				deleteItem((*iter)->internalId(), internalId);
			}
		}

		if (it->type() == SNDevTreeItem::Device)
		{
			stopDevice(static_cast<SNDevTreeDeviceItem *>(it)->devId());

			set<SNMapItem *>::iterator mapItem = m_mapItems.find(it->mapItem());
			if (mapItem != m_mapItems.end())
			{
				if (it->type() == SNDevTreeItem::Device)
				{
					SNMapDeviceItem *device = static_cast<SNMapDeviceItem *>(*mapItem);
					uint32_t deviceId = device->deviceId();
					std::map<uint32_t, SNMapDeviceItem *>::iterator deviceIterator = m_devices.find(deviceId);
					if (deviceIterator != m_devices.end())
					{
						m_devices.erase(deviceIterator);
					}
					m_scene->removeDevice(device);
				}
				m_mapItems.erase(mapItem);
				delete (*mapItem);
			}
		}
		m_items.erase(itemIter);
	}

	SNDevTreeNode *node = tree->second;
	node->removeItem(internalId);
	if (node->childCount() == 0)
	{
		delete node;
		m_itemsTree.erase(tree);
	}
}

/*!
  Odstranenie jedinej polozky s internym ID \a internalId (moze byt aj adresar)
  z adresara \a parent. Podpolozky zoztanu nezmenene vratane nadradenych
  poloziek. Tato funkcia sa pouziva pri presuvani adresarov. Polozka sa
  neodstrani ani z interneho zoznamu aby sa dala znovu vlozit pomocou
  insertItem.

  \sa insertItem
*/
void SNMap::removeItem(uint32_t internalId, uint32_t parent)
{
	// najdenie stromu
	map<uint32_t, SNDevTreeNode *>::iterator tree;
	tree = m_itemsTree.find(parent);
	if (tree == m_itemsTree.end())
	{
		return;
	}

	SNDevTreeNode *node = tree->second;
	node->removeItem(internalId, false);
	if (node->childCount() == 0)
	{
		delete node;
		m_itemsTree.erase(tree);
	}
}

/*!
  Vlozenie uz existujucej polozky \a internalId do adresara \a parent na riadok
  \a pos. Tato polozka musi byt najskor zrusena pomocou removeItem.

  \sa removeItem
*/
void SNMap::insertItem(uint32_t internalId, uint32_t parent, int pos)
{
	map<uint32_t, SNDevTreeItem *>::iterator itemIter;
	itemIter = m_items.find(internalId);
	if (itemIter == m_items.end())
	{
		return;
	}

	SNDevTreeItem *item = itemIter->second;
	map<uint32_t, SNDevTreeNode*>::iterator it;
	if (m_itemsTree.find(parent) == m_itemsTree.end())
	{
		if (parent == 0)
		{
			m_itemsTree[parent] = new SNDevTreeNode(NULL);
		}
		else
		{
			m_itemsTree[parent] = new SNDevTreeNode(m_items[parent]);
		}
	}
	m_itemsTree[parent]->insert(item, pos);
}

/*!
  Vrati zoznam poloziek v podstrome urcenom internym ID \a parent.
*/
std::list<SNDevTreeItem *> SNMap::itemsInTree(uint32_t parent) const
{
	list<SNDevTreeItem *> ret;

	map<uint32_t, SNDevTreeItem *>::const_iterator itemIter;
	itemIter = m_items.find(parent);
	if (itemIter == m_items.end())
	{
		return ret;
	}
	ret.push_back(itemIter->second);

	map<uint32_t, SNDevTreeNode*>::const_iterator node;
	node = m_itemsTree.find(parent);
	if (node != m_itemsTree.end())
	{
		vector<SNDevTreeItem *> *childs = node->second->childs();
		vector<SNDevTreeItem *>::iterator child;
		for (child = childs->begin(); child != childs->end(); ++child)
		{
			list<SNDevTreeItem *> items = itemsInTree((*child)->internalId());
			ret.merge(items);
		}
	}

	return ret;
}

/*!
  Vrati zariadenie asociovane s ID zariadenia \a devId.
*/
SNDevice *SNMap::device(uint32_t devId)
{
	return m_simulate->device(devId);
}

/*!
  Nastavenie sceny v ktorej sa vykresluje mapa.
*/
void SNMap::setScene(SNAbstractDevicesScene *scene)
{
	m_scene = scene;
	scene->setMap(this);
}

/*!
  Zistenie scene v ktorej sa vykresluje mapa.
*/
SNAbstractDevicesScene *SNMap::scene() const
{
	return m_scene;
}

/*!
  Pridanie noveho portu zariadeniu \a devId.
*/
void SNMap::insertPort(uint32_t devId, port_num hwPort)
{
	if (m_scene != NULL)
	{
		std::map<uint32_t, SNMapDeviceItem *>::iterator device;
		device = m_devices.find(devId);
		if (device != m_devices.end())
		{
			m_scene->addHwPort(device->second, hwPort);
		}
	}
}

/*!
  Odstranenie portu zo zariadenia \a devId.
*/
void SNMap::removePort(uint32_t devId, port_num hwPort)
{
	if (m_scene != NULL)
	{
		std::map<uint32_t, SNMapDeviceItem *>::iterator device;
		device = m_devices.find(devId);
		if (device != m_devices.end())
		{
			m_scene->removeHwPort(device->second, hwPort);
		}
	}
}

void SNMap::updateDevice(SNMapDeviceItem *device)
{
	if (m_scene != NULL)
	{
		m_scene->updateDevice(device);
	}
}

SNMapLineItem *SNMap::addLine()
{
	SNMapLineItem *line = new SNMapLineItem;
	m_lines.insert(line);
	return line;
}

void SNMap::removeLine(SNMapLineItem *line)
{
	set<SNMapLineItem *>::iterator lineIter;
	lineIter = m_lines.find(line);
	if (lineIter != m_lines.end())
	{
		m_lines.erase(lineIter);
		delete line;
	}
}
