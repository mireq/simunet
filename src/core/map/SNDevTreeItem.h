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
		uint32_t id() const;
		SNDevTreeItem *parent() const;
		uint32_t parentId() const;
		int type() const;

		void setParent(SNDevTreeItem *parent);
		void setId(uint32_t id);

		void setMapItem(SNMapItem *mapItem);
		SNMapItem *mapItem() const;

	protected:
		SNMapItem *m_mapItem;

	private:
		static uint32_t m_nextId;

		uint32_t m_internalId;
		uint32_t m_id;
		SNDevTreeItem *m_parent;
		ItemType m_type;
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
		void setDevId(uint32_t devId);
};

#endif
