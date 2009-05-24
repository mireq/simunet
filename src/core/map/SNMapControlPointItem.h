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
#ifndef SNMAPCONTROLPOINT_H
#define SNMAPCONTROLPOINT_H

#include "SNMapItem.h"
#include "sntypes.h"

class SNMapDeviceItem;

/**
 @author Miroslav Bendik
*/
class SNMapControlPointItem: public SNMapItem
{
	public:
		/// Typ kontorlneho bodu.
		enum ControlPointType {
			ControlPoint, /*!< Standardny kontrolny bod na miestac spojov segmentov ciar. */
			Connector /*!< Konektor na zariadeni. */
		};
		SNMapControlPointItem(const SNPoint3f &pos, ControlPointType type = ControlPoint);
		~SNMapControlPointItem();
		int type() const;
	private:
/*!
  Typ kontrolneho bodu

  \sa SNMapControlPointItem::ControlPointType
*/
		int m_type;
};

/**
 @author Miroslav Bendik
 */
class SNMapConnectorItem: public SNMapControlPointItem
{
	public:
		SNMapConnectorItem(const SNPoint3f &pos, SNMapDeviceItem *device, port_num port);
		~SNMapConnectorItem();
		SNMapDeviceItem *device() const;
		port_num port() const;

	private:
/*!
  Odkaz na zariadenie, ktoremu patri konektor.
*/
		SNMapDeviceItem *m_device;
/*!
  Cislo konektoru na zariadeni.
*/
		port_num m_port;
};

#endif
