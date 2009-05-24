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
#ifndef SNMAPLINEITEM_H
#define SNMAPLINEITEM_H

#include <vector>
#include <set>
#include "sntypes.h"

class SNMap;
class SNMapControlPointItem;
class SNMapDeviceItem;
class SNMapConnectorItem;

/**
 @author Miroslav Bendik
*/
class SNMapLineItem
{
	public:
		SNMapLineItem(SNMap *map);
		~SNMapLineItem();
		void addConnector(float x, float y, float z, SNMapDeviceItem *device, port_num port, std::vector<SNMapControlPointItem *>::size_type pos);
		void addControlPoint(float x, float y, float z, std::vector<SNMapControlPointItem *>::size_type pos);
		void addControlPoint(SNMapControlPointItem *point, std::vector<SNMapControlPointItem *>::size_type pos);
		void removeControlPoint(std::vector<SNMapControlPointItem *>::size_type pos);
	private:
/*!
  Zoznam kontrolnych bodov prepojenia.
*/
		std::vector<SNMapControlPointItem *>m_controlPoints;

/*!
  Zoznam konektorov.
*/
		std::set<SNMapConnectorItem *>m_connectors;

/*!
  Odkaz na mapu.
*/
		SNMap *m_map;

};

/**
 @author Miroslav Bendik
 */
class SNMapConnection
{
	public:
		SNMapConnection();
		~SNMapConnection();
		void setPort1(port_num port1);
		void setPort2(port_num port2);
		void setDev1(SNMapDeviceItem *dev1);
		void setDev2(SNMapDeviceItem *dev2);
		void setConn1(SNMapDeviceItem *dev1, port_num port1);
		void setConn2(SNMapDeviceItem *dev2, port_num port2);
		port_num port1() const;
		port_num port2() const;
		SNMapDeviceItem *dev1() const;
		SNMapDeviceItem *dev2() const;
		bool isValid() const;
	private:
/*!
  Odkaz na prve zariadenie.
*/
		SNMapDeviceItem *m_dev1;
/*!
  Odkaz na druhe zariadenie.
*/
		SNMapDeviceItem *m_dev2;
/*!
  Cislo portu prveho zariadenia.
*/
		port_num m_port1;
/*!
  Cislo portu druheho zariadenia.
*/
		port_num m_port2;
};

#endif
