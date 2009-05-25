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

#include "gui/noqobject/SNAbstractDevicesScene.h"

#include "SNDevTreeItem.h"
#include "SNDevTreeNode.h"
#include "SNMapDeviceItem.h"
#include "SNMapLineItem.h"

#include "core/SNAccessors.h"
#include "core/SNConfig.h"
#include "core/SNSimulate.h"

#include <vector>

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
	m_rootDir = new SNDevTreeNode(NULL);
	SNConfig config;
	m_simulate = SNSingleton::getSimulate();
	m_simulate->setMap(this);
}

/*!
  Zrusenie mapy.
*/
SNMap::~SNMap()
{
	while (m_rootDir->childCount() > 0)
	{
		dealloc(*(m_rootDir->childs()->begin()), NULL);
	}
	delete m_rootDir;
	m_rootDir = NULL;

	while (m_lines.begin() != m_lines.end())
	{
		delete (*m_lines.begin());
		m_lines.erase(m_lines.begin());
	}
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
  argumentom \a parent. Argument \a pos urcuje poradie, v ktorom sa vlozi
  zariadenie.
*/
void SNMap::insertDevice(uint32_t devId, SNDevTreeItem *parent, int pos)
{
	SNMapDeviceItem *device = new SNMapDeviceItem(devId);
	m_mapItems.insert(device);
	m_devices[devId] = device;

	SNDevTreeNode *node = NULL;
	SNDevTreeDeviceItem *item = new SNDevTreeDeviceItem(device);
	if (parent == NULL)
	{
		node = m_rootDir;
	}
	else
	{
		if (parent->node() == NULL)
		{
			parent->setNode(new SNDevTreeNode(parent));
		}
		node = parent->node();
	}
	node->insert(item, pos);

	m_scene->addDevice(device);
}

/*!
  Vytvorenie noveho adresara s menom \a name v adresari \a parent. Ak je
  nadradeny adresar \a parent rovny 0 prida sa adresar do korenoveho adresara.
  Riadok, do ktoreho sa vlozi novy adresar je urceny argumentom \a pos.
*/
void SNMap::addDirectory(std::string name, SNDevTreeItem *parent, int pos)
{
	SNDevTreeDirectoryItem *item = new SNDevTreeDirectoryItem(name);
	SNDevTreeNode *node = NULL;
	if (parent == NULL)
	{
		node = m_rootDir;
	}
	else
	{
		if (parent->node() == NULL)
		{
			parent->setNode(new SNDevTreeNode(parent));
		}
		node = parent->node();
	}
	node->insert(item, pos);
}

/*!
  Vrati pocet poloziek v adresari urcenom paremetrom \a parent.
*/
int SNMap::numItems(SNDevTreeItem *parent) const
{
	if (parent == NULL)
	{
		return m_rootDir->childCount();
	}

	SNDevTreeNode *node = parent->node();
	if (node == 0)
	{
		return 0;
	}
	else
	{
		return node->childCount();
	}
}

/*!
  Vrati polozku stromu zaraideni, ktora sa nachadza v riadku urcenom argumentom
  \a pos nachadzajucu sa v adresari urcenom argumentom \a parent.
*/
SNDevTreeItem *SNMap::itemAt(int pos, SNDevTreeItem *parent) const
{
	SNDevTreeNode *node = NULL;
	if (parent == NULL)
	{
		node = m_rootDir;
	}
	else
	{
		node = parent->node();
	}
	if (node == NULL)
	{
		return NULL;
	}
	return node->itemAt(pos);
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
  Vrati riadok polozky \a item nachadzajucej sa v adresari \a parent.
*/
int SNMap::itemIndex(SNDevTreeItem *item, SNDevTreeItem *parent) const
{
	SNDevTreeNode *node = NULL;
	if (parent == NULL)
	{
		node = m_rootDir;
	}
	else
	{
		node = parent->node();
	}

	if (node == NULL)
	{
		return -1;
	}
	return node->index(item->internalId());
}

/*!
  Odstranenie polozky alebo celeho adresara vratane vsetkych podpoloziek
  v zozname zariadeni. Polozka, ktora sa ma odstranit je urcena argumentom
  \a item. Specifikovat sa musi aj nadradeny adresar (pripadne \b NULL v
  pripade korenoveho adresara), v ktoom sa polozka nachadza.
*/
void SNMap::deleteItem(SNDevTreeItem *item, SNDevTreeItem *parent)
{
	SNDevTreeNode *tree = NULL;
	if (parent == NULL)
	{
		tree = m_rootDir;
	}
	else
	{
		tree = parent->node();
	}

	SNDevTreeNode *subtree = item->node();
	if (subtree != NULL)
	{
		vector<SNDevTreeItem *> *childs = subtree->childs();
		vector<SNDevTreeItem *>::iterator childIt;
		list<SNDevTreeItem *> itemsToDelete;
		for (childIt = childs->begin(); childIt != childs->end(); ++childIt)
		{
			itemsToDelete.push_back(*childIt);
		}
		list<SNDevTreeItem *>::iterator iter;
		for (iter = itemsToDelete.begin(); iter != itemsToDelete.end(); ++iter)
		{
			deleteItem((*iter), item);
		}
	}

	if (item->type() == SNDevTreeItem::Device)
	{
		stopDevice(static_cast<SNDevTreeDeviceItem *>(item)->devId());

		set<SNMapItem *>::iterator mapItem = m_mapItems.find(item->mapItem());
		if (mapItem != m_mapItems.end())
		{
			SNMapDeviceItem *device = static_cast<SNMapDeviceItem *>(*mapItem);
			m_scene->removeDevice(device);
			uint32_t deviceId = device->deviceId();
			std::map<uint32_t, SNMapDeviceItem *>::iterator deviceIterator = m_devices.find(deviceId);
			if (deviceIterator != m_devices.end())
			{
				m_devices.erase(deviceIterator);
			}

			m_mapItems.erase(mapItem);
			delete device;
		}
	}

	tree->removeItem(item->internalId());
	if (tree == m_rootDir)
	{
		return;
	}

	if (tree->childCount() == 0)
	{
		tree->treeItem()->setNode(NULL);
		delete tree;
	}
}

/*!
  Odstranenie jedinej polozky \a item (moze byt aj adresar)
  z adresara \a parent. Podpolozky zoztanu nezmenene vratane nadradenych
  poloziek. Tato funkcia sa pouziva pri presuvani adresarov. Polozka sa
  neodstrani ani z interneho zoznamu aby sa dala znovu vlozit pomocou
  insertItem.

  \sa insertItem
*/
void SNMap::removeItem(SNDevTreeItem *item, SNDevTreeItem *parent)
{
	SNDevTreeNode *node = NULL;
	if (parent == NULL)
	{
		node = m_rootDir;
	}
	else
	{
		node = parent->node();
	}

	node->removeItem(item->internalId(), false);
	if (node->childCount() == 0)
	{
		node->treeItem()->setNode(NULL);
		delete node;
	}
}

/*!
  Vlozenie uz existujucej polozky \a idtem do adresara \a parent na riadok
  \a pos. Tato polozka musi byt najskor zrusena pomocou removeItem.

  \sa removeItem
*/
void SNMap::insertItem(SNDevTreeItem *item, SNDevTreeItem *parent, int pos)
{
	SNDevTreeNode *node = NULL;
	if (parent == NULL)
	{
		node = m_rootDir;
	}
	else
	{
		if (parent->node() == NULL)
		{
			parent->setNode(new SNDevTreeNode(parent));
		}
		node = parent->node();
	}
	node->insert(item, pos);
}

/*!
  Vrati zoznam poloziek v podstrome \a parent.
*/
std::list<SNDevTreeItem *> SNMap::itemsInTree(SNDevTreeItem *parent) const
{
	list<SNDevTreeItem *> ret;
	if (parent == NULL)
	{
		return ret;
	}
	ret.push_back(parent);

	SNDevTreeNode *node = parent->node();
	if (node == NULL)
	{
		return ret;
	}

	vector<SNDevTreeItem *> *childs = node->childs();
	vector<SNDevTreeItem *>::iterator child;
	for (child = childs->begin(); child != childs->end(); ++child)
	{
		list<SNDevTreeItem *> items = itemsInTree(*child);
		ret.merge(items);
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

/*!
  Aktualizacia vlastnosti zariadenia (znovu sa nacitaju napr. porty).
*/
void SNMap::updateDevice(SNMapDeviceItem *device)
{
	if (m_scene != NULL)
	{
		m_scene->updateDevice(device);
	}
}

/*!
  Vytvorenie noveho prepojenia.
*/
SNMapLineItem *SNMap::addLine()
{
	SNMapLineItem *line = new SNMapLineItem(this);
	m_lines.insert(line);
	return line;
}

/*!
  Odstranenie prepojenia.
*/
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

/*!
  Vytvorenie fyzickeho prepojenia medzi zariadeniami.
*/
void SNMap::addConnection(const SNMapConnection &conn)
{
	m_simulate->addConnection(conn.dev1()->deviceId(), conn.port1(), conn.dev2()->deviceId(), conn.port2());
}

/*!
  Zrusenie prepojenia medzi zariadeniami.
*/
void SNMap::removeConnection(const SNMapConnection &conn)
{
	m_simulate->removeConnection(conn.dev1()->deviceId(), conn.port1(), conn.dev2()->deviceId(), conn.port2());
}

void SNMap::dealloc(SNDevTreeItem *item, SNDevTreeItem *parent)
{
	SNDevTreeNode *tree = NULL;
	if (parent == NULL)
	{
		tree = m_rootDir;
	}
	else
	{
		tree = parent->node();
	}

	SNDevTreeNode *subtree = item->node();
	if (subtree != NULL)
	{
		while (subtree->childCount() != 0)
		{
			dealloc(*(subtree->childs()->begin()), item);
		}
	}

	if (item->type() == SNDevTreeItem::Device)
	{
		set<SNMapItem *>::iterator mapItem = m_mapItems.find(item->mapItem());
		if (mapItem != m_mapItems.end())
		{
			SNMapDeviceItem *device = static_cast<SNMapDeviceItem *>(*mapItem);
			uint32_t deviceId = device->deviceId();
			std::map<uint32_t, SNMapDeviceItem *>::iterator deviceIterator = m_devices.find(deviceId);
			if (deviceIterator != m_devices.end())
			{
				m_devices.erase(deviceIterator);
			}

			m_mapItems.erase(mapItem);
			delete device;
		}
	}

	tree->removeItem(item->internalId());
	if (tree == m_rootDir)
	{
		return;
	}

	if (tree->childCount() == 0)
	{
		tree->treeItem()->setNode(NULL);
		delete tree;
	}
}
