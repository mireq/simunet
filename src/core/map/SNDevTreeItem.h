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
#ifndef SNDEVTREEITEM_H
#define SNDEVTREEITEM_H

#include <stdint.h>
#include <string>

class SNMapItem;
class SNMapDeviceItem;
class SNDevTreeNode;

/**
 @author Miroslav Bendik
*/
class SNDevTreeItem
{
	public:

/*!
  Typ polozky stromovej struktury.
*/
		enum ItemType
		{
			Device, /*!< Zariadenie */
			Directory /*!< Adresar */
		};

		SNDevTreeItem(ItemType type = Device, SNMapItem *item = 0, SNDevTreeItem *parent = 0);
		virtual ~SNDevTreeItem();

		uint32_t internalId() const;
		SNDevTreeItem *parent() const;
		uint32_t parentId() const;
		int type() const;

		void setParent(SNDevTreeItem *parent);

		void setMapItem(SNMapItem *mapItem);
		SNMapItem *mapItem() const;

		void setNode(SNDevTreeNode *node);
		SNDevTreeNode *node() const;

	protected:
/*!
  Odkaz na polozku mapy.
*/
		SNMapItem *m_mapItem;

	private:
/*!
  Nasledujuce ID polozky zoznamu zariadeni.
*/
		static uint32_t m_nextId;

/*!
  Interne ID polozky zoznamu zariadeni.
*/
		uint32_t m_internalId;

/*!
  Odkaz na nadradenu polozku.
*/
		SNDevTreeItem *m_parent;

/*!
  Typ polozky zoznamu zariadeni (zariadenie, adresar ...)
*/
		ItemType m_type;

/*!
  Odkaz na uzol u polozky zoznamu.
*/
		SNDevTreeNode *m_node;
};

/* ----------------------------------------------------------------------- */

/**
 @author Miroslav Bendik
 */
class SNDevTreeDirectoryItem: public SNDevTreeItem
{
	public:
		SNDevTreeDirectoryItem(const std::string &name, SNMapItem *item = 0, SNDevTreeItem *parent = 0);
		~SNDevTreeDirectoryItem();
		std::string name() const;
		void setName(const std::string &name);
	private:
/*!
  Nazov adresara.
*/
		std::string m_name;
};

/* ----------------------------------------------------------------------- */

/**
 @author Miroslav Bendik
 */
class SNDevTreeDeviceItem: public SNDevTreeItem
{
	public:
		SNDevTreeDeviceItem(SNMapDeviceItem *item = 0, SNDevTreeItem *parent = 0);
		~SNDevTreeDeviceItem();
		uint32_t devId() const;
		SNMapDeviceItem *mapDevice() const;
};

#endif
