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
#include "SNDevTreeNode.h"
#include "SNDevTreeItem.h"

/*!
  \class SNDevTreeNode
  \brief Uzol v stromovom zozname zariadeni.
  \ingroup map
 */

/*!
  Vytvorenie noveho uzlu v adresari \a parent.
*/
SNDevTreeNode::SNDevTreeNode(SNDevTreeItem *parent)
	: m_parent(parent), m_childs(NULL)
{
}

/*!
  Zrusenie uzlu a odstranenie vsetkych poloziek v uzle.
*/
SNDevTreeNode::~SNDevTreeNode()
{
	if (m_childs != NULL)
	{
		delete m_childs;
		m_childs = NULL;
	}
}

/*!
  Vlozenie novej polozky \a item do zoznamu poloziek v aktualnom uzle
  na riadok urceny argumentom \a pos.
*/
void SNDevTreeNode::insert(SNDevTreeItem *item, int pos)
{
	if (m_childs == NULL)
	{
		m_childs = new std::vector<SNDevTreeItem *>;
	}

	if (pos == -1)
	{
		pos = m_childs->size();
	}

	item->setParent(m_parent);

	m_childs->insert(m_childs->begin() + pos, item);
}

/*!
  Vrati polozku v riadku \a pos v aktualnom uzle.
*/
SNDevTreeItem *SNDevTreeNode::itemAt(int pos) const
{
	if (pos >= (int)m_childs->size())
	{
		return NULL;
	}
	else
	{
		return (*m_childs)[pos];
	}
}

/*!
  Vrati pocet poloziek v tomto uzle.
*/
int SNDevTreeNode::childCount() const
{
	if (m_childs == NULL)
	{
		return 0;
	}
	return m_childs->size();
}

/*!
  Vrati zoznam poloziek v tomto uzle.
*/
std::vector<SNDevTreeItem *> *SNDevTreeNode::childs() const
{
	return m_childs;
}

/*!
  Vrati cilo riadku v ktorom sa nachadza polozka s internym ID urcenym
  argumentom \a id.
*/
int SNDevTreeNode::index(uint32_t id) const
{
	int index = 0;
	std::vector<SNDevTreeItem *>::const_iterator it;
	for (it = m_childs->begin(); it != m_childs->end(); ++it)
	{
		if ((*it)->internalId() == id)
		{
			return index;
		}
		index++;
	}
	return -1;
}

/*!
  Odstranenie polozky urcenej internym ID \a id z tohto uzla. Ak je argument
  \a del \e true potom sa tato polozka dealokuje.
*/
void SNDevTreeNode::removeItem(uint32_t id, bool del)
{
	std::vector<SNDevTreeItem *>::iterator it;
	for (it = m_childs->begin(); it != m_childs->end(); ++it)
	{
		if ((*it)->internalId() == id)
		{
			break;
		}
	}

	if (it == m_childs->end())
	{
		return;
	}

	SNDevTreeItem *item = *it;
	(*it)->setParent(NULL);
	m_childs->erase(it);
	if (del)
	{
		delete item;
	}
}

SNDevTreeItem *SNDevTreeNode::parent() const
{
	return m_parent;
}


