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
#include "SNMapControlPointItem.h"

/*!
  \class SNMapControlPointItem
  \brief Kontrolny bod, cez ktory prechadza ciara spajajuca zariadenia.
  \ingroup map
 */

/*!
  Vytvorenie noveho kontrolneho bodu na pozicii urcenej argumentom \a pos
  typom \a type.
*/
SNMapControlPointItem::SNMapControlPointItem(const SNPoint3f &pos, ControlPointType type)
	: SNMapItem(pos), m_type(type)
{
}

/*!
  Zrusenie kontrolneho bodu.
*/
SNMapControlPointItem::~SNMapControlPointItem()
{
}

/*!
  Zistenie typu kontrolneho bodu.
*/
int SNMapControlPointItem::type() const
{
	return m_type;
}


/* ------------------------------------------------------------------ */

/*!
  \class SNMapConnectorItem
  \brief Koncove kontrolne body (konektory).
  \ingroup map
 */

/*!
  Vytvorenie konektoru na pozicii urcenej argumentom \a pos. Konektor patri
  zariadeniu \a device a jeho cislo je urcene argumentom \a port.
*/
SNMapConnectorItem::SNMapConnectorItem(const SNPoint3f &pos, SNMapDeviceItem *device, port_num port)
	: SNMapControlPointItem(pos, Connector)
{
	m_device = device;
	m_port = port;
}

/*!
  Zrusenie konektoru.
*/
SNMapConnectorItem::~ SNMapConnectorItem()
{
}

/*!
  Ziskanie referencie na zariadenie, ktoremu patri konektor.
*/
SNMapDeviceItem *SNMapConnectorItem::device() const
{
	return m_device;
}

/*!
  Zistenie cisla portu.
*/
port_num SNMapConnectorItem::port() const
{
	return m_port;
}


