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
#include "SNDevTreeItem.h"
#include "SNMapItem.h"
#include "SNMapDeviceItem.h"

#include <QDebug>

/*!
  ID nasledujuceho interneho ID.
*/
uint32_t SNDevTreeItem::m_nextId = 1;

/*!
  \class SNDevTreeItem
  \brief Zakladna trieda od ktorej su odvadzane jednotlive polzky zoznamu zariadeni.
  \ingroup map
 */

/*!
  Vytvorenie novej polozky s typom urcenym argumentom \a type s nadradenou
  polozkou \a parent. Ak sa zavola bez uvedenia nadreadenej polozky znamena
  to, ze polozka je v korenovom adresari.
*/
SNDevTreeItem::SNDevTreeItem(ItemType type, SNMapItem *mapItem, SNDevTreeItem *parent)
	: m_node(0)
{
	m_type = type;
	m_mapItem = mapItem;
	m_parent = parent;
	m_internalId = m_nextId;
	m_nextId++;
}

/*!
  Zrusenie polozky zoznamu zariadeni.
*/
SNDevTreeItem::~SNDevTreeItem()
{
	m_mapItem = NULL;
	m_parent  = NULL;
	m_node    = NULL;
}

/*!
  Zistenie unikatneho interneho ID.
*/
uint32_t SNDevTreeItem::internalId() const
{
	return m_internalId;
}



/*!
  Zistenie nadreadenej polozky stromovej struktury. V pripade, ze taka neexistuje
  vrati NULL.

  \sa parentId
*/
SNDevTreeItem *SNDevTreeItem::parent() const
{
	return m_parent;
}

/*!
  Zistenie interneho ID nadradenej polozky. Ak taka neexistuje (napr. polozka
  sa nachadza v korenovom adresari) vrati 0.

  \overload

  \sa parent
*/
uint32_t SNDevTreeItem::parentId() const
{
	if (m_parent == NULL)
	{
		return 0;
	}
	else
	{
		return m_parent->internalId();
	}
}

/*!
  Zistenie typu polozky. Okrem uz preddefinovanych typov je mozne pouzit aj vlastne
  (preto ma navratovy typ int).
*/
int SNDevTreeItem::type() const
{
	return m_type;
}

/*!
  Nastavenie nadradenej polzoky na novu polozku \a parent.
*/
void SNDevTreeItem::setParent(SNDevTreeItem *parent)
{
	m_parent = parent;
}


/*!
  Nastavenie odkazu na polozku mapy.

  \sa mapItem
*/
void SNDevTreeItem::setMapItem(SNMapItem *mapItem)
{
	m_mapItem = mapItem;
}

/*!
  Zistenie polozky menu, ktora je spojena s touto polozkou zoznamu zariadeni.

  \sa setMapItem
*/
SNMapItem *SNDevTreeItem::mapItem() const
{
	return m_mapItem;
}

/*!
  Nastavenie uzlu v danej polozke zoznamu zariadeni.

  \sa node
*/
void SNDevTreeItem::setNode(SNDevTreeNode *node)
{
	m_node = node;
}

/*!
  Zistenie uzlu v polozke zoznamu zariadeni. Ak je pocet podpoloziek nulovy
  moze byt uzol nealokovany (NULL).

  \sa setNode
*/
SNDevTreeNode *SNDevTreeItem::node() const
{
	return m_node;
}


/* ----------------------------------------------------------------------- */

/*!
  \class SNDevTreeDirectoryItem
  \brief Polozka zoznamu zariadeni reprezentujuca adresar.
  \ingroup map
 */

/*!
  Vytvorenie noveho adresara.

  \param name Nazov noveho adresara.
  \param mapItem Polozka mapy, ktorej prislucha polozka zonznamu zariadeni.
  \param parent Nadradena polozka zoznamu zariadeni.
*/
SNDevTreeDirectoryItem::SNDevTreeDirectoryItem(const std::string &name, SNMapItem *mapItem, SNDevTreeItem *parent)
	: SNDevTreeItem(Directory, mapItem, parent)
{
	m_name = name;
}

/*!
  Zrusenie adresara.
*/
SNDevTreeDirectoryItem::~ SNDevTreeDirectoryItem()
{
}

/*!
  Zistenie nazvu adresara.
*/
std::string SNDevTreeDirectoryItem::name() const
{
	return m_name;
}

/*!
  Nastavenie nazvu adresara.
*/
void SNDevTreeDirectoryItem::setName(const std::string &name)
{
	m_name = name;
}

/* ----------------------------------------------------------------------- */

/*!
  \class SNDevTreeDeviceItem
  \brief Polozka zoznamu zariadeni reprezentujuca zariadenie.
  \ingroup map
 */

/*!
  Vytvorenie noveho zariadenia v zozname zariadeni.

  \param mapItem Polozka mapy so zariadeniami. Z tejto polozky sa ziskavaju informacie ako napr. ID zariadenia.
  \param parent Nadradena polozka zoznamu zariadeni.
*/
SNDevTreeDeviceItem::SNDevTreeDeviceItem(SNMapDeviceItem *mapItem, SNDevTreeItem *parent)
	: SNDevTreeItem(Device, mapItem, parent)
{
}

/*!
  Zrusenie polozky zoznamu zariadeni.
*/
SNDevTreeDeviceItem::~SNDevTreeDeviceItem()
{
}

/*!
  Zistenie ID zariadenia.
*/
uint32_t SNDevTreeDeviceItem::devId() const
{
	if (m_mapItem != NULL)
	{
		return static_cast<SNMapDeviceItem *>(m_mapItem)->deviceId();
	}
	return 0;
}

/*!
  Ziskanie referencie na zariadenie z mapy so zariadeniami.
*/
SNMapDeviceItem *SNDevTreeDeviceItem::mapDevice() const
{
	return static_cast<SNMapDeviceItem *>(mapItem());
}


