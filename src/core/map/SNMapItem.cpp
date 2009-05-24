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
#include "SNMapItem.h"

/*!
  \class SNMapItem
  \brief Polozka na mape so zariadeniami.
  \ingroup map

  Tato trieda ma za ulohu uchovavat informacie o polozkach na mape. Simulacne
  jadro nema ziadne informacie o polohe jednotlivych zariadeni.
*/

/*!
  Vytvorenie novej polozky mapy na pozicii (0, 0, 0).
*/
SNMapItem::SNMapItem()
	: m_pos(SNPoint3f(0.0, 0.0, 0.0))
{
}

/*!
  Vytvorenie novej polozky mapy na pozicii \a pos.
*/
SNMapItem::SNMapItem(const SNPoint3f &pos)
	: m_pos(pos)
{
}

/*!
  Zrusenie polozky mapy.
*/
SNMapItem::~SNMapItem()
{
}

/*!
  Nastavenie polohy polozky v 3D priestore.
*/
void SNMapItem::setPos(const SNPoint3f &pos)
{
	if (m_pos != pos)
	{
		m_pos = pos;
	}
}

/*!
  Zistenie polohy polozky v 3D priestore.
*/
SNPoint3f SNMapItem::pos() const
{
	return m_pos;
}


