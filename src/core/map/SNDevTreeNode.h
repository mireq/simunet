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
#ifndef SNDEVTREENODE_H
#define SNDEVTREENODE_H

#include <vector>
#include <stdint.h>


class SNDevTreeItem;

/**
 @author Miroslav Bendik
*/
class SNDevTreeNode
{
	public:
		SNDevTreeNode(SNDevTreeItem *parent);
		~SNDevTreeNode();
		void insert(SNDevTreeItem *item, int pos = -1);
		SNDevTreeItem *itemAt(int pos) const;
		int childCount() const;
		std::vector<SNDevTreeItem *> *childs() const;
		int index(uint32_t id) const;
		void removeItem(uint32_t id, bool del = true);

	private:
		SNDevTreeItem *m_parent;
		std::vector<SNDevTreeItem *> *m_childs;

};

#endif
