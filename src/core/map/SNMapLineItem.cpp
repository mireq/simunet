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
#include "SNMapLineItem.h"
#include "SNMapControlPointItem.h"
#include "SNMapDeviceItem.h"

#include "core/SNExceptions.h"

#include <QDebug>

using namespace std;

SNMapLineItem::SNMapLineItem(SNMap *map)
{
	m_map = map;
}


SNMapLineItem::~SNMapLineItem()
{
	vector<SNMapControlPointItem *>::iterator controlPoint;
	for (controlPoint = m_controlPoints.begin(); controlPoint != m_controlPoints.end(); ++controlPoint)
	{
		delete (*controlPoint);
	}
}

void SNMapLineItem::addConnector(float x, float y, float z, SNMapDeviceItem *device, port_num port, std::vector< SNMapControlPointItem * >::size_type pos)
{
	addControlPoint(new SNMapConnectorItem(SNPoint3f(x, y, z), device, port), pos);
}

void SNMapLineItem::addControlPoint(float x, float y, float z, std::vector<SNMapControlPointItem *>::size_type pos)
{
	addControlPoint(new SNMapControlPointItem(SNPoint3f(x, y, z)), pos);
}

void SNMapLineItem::addControlPoint(SNMapControlPointItem *point, std::vector< SNMapControlPointItem * >::size_type pos)
{
	if (pos > m_controlPoints.size())
	{
		throw SNoutOfRangeException();
	}

	m_controlPoints.insert(m_controlPoints.begin() + pos, point);

	if (point->type() == SNMapControlPointItem::Connector)
	{
		// pridanie spojenia
		if (m_connectors.size() == 1)
		{
			SNMapConnectorItem *conn1 = *(m_connectors.begin());
			SNMapConnectorItem *conn2 = static_cast<SNMapConnectorItem *>(point);
			SNMapConnection conn;
			conn.setConn1(conn1->device(), conn1->port());
			conn.setConn2(conn2->device(), conn2->port());
			m_map->addConnection(conn);
		}
		m_connectors.insert(static_cast<SNMapConnectorItem *>(point));
	}
}

void SNMapLineItem::removeControlPoint(std::vector<SNMapControlPointItem *>::size_type pos)
{
	if (pos >= m_controlPoints.size())
	{
		throw SNoutOfRangeException();
	}

	std::vector<SNMapControlPointItem *>::iterator pointIter = m_controlPoints.begin() + pos;
	SNMapControlPointItem *point = *pointIter;

	if (point->type() == SNMapControlPointItem::Connector)
	{
		std::set<SNMapConnectorItem *>::iterator connIter;
		connIter = m_connectors.find(static_cast<SNMapConnectorItem *>(point));
		if (connIter != m_connectors.end())
		{
			m_connectors.erase(connIter);
			// odstranenie spojenia
			if (m_connectors.size() == 1)
			{
				SNMapConnectorItem *conn1 = *(m_connectors.begin());
				SNMapConnectorItem *conn2 = static_cast<SNMapConnectorItem *>(point);
				SNMapConnection conn;
				conn.setConn1(conn1->device(), conn1->port());
				conn.setConn2(conn2->device(), conn2->port());
				m_map->removeConnection(conn);
			}
		}
	}

	m_controlPoints.erase(pointIter);
	delete point;
}

/* ------------------------------------------------------------------ */

SNMapConnection::SNMapConnection()
	: m_dev1(NULL), m_dev2(NULL), m_port1(0), m_port2(0)
{
}

SNMapConnection::~SNMapConnection()
{
}

void SNMapConnection::setPort1(port_num port1)
{
	m_port1 = port1;
}

void SNMapConnection::setPort2(port_num port2)
{
	m_port2 = port2;
}

void SNMapConnection::setDev1(SNMapDeviceItem *dev1)
{
	m_dev1 = dev1;
}

void SNMapConnection::setDev2(SNMapDeviceItem *dev2)
{
	m_dev2 = dev2;
}

void SNMapConnection::setConn1(SNMapDeviceItem *dev1, port_num port1)
{
	m_dev1 = dev1;
	m_port1 = port1;
}

void SNMapConnection::setConn2(SNMapDeviceItem * dev2, port_num port2)
{
	m_dev2 = dev2;
	m_port2 = port2;
}

port_num SNMapConnection::port1() const
{
	return m_port1;
}

port_num SNMapConnection::port2() const
{
	return m_port2;
}

SNMapDeviceItem *SNMapConnection::dev1() const
{
	return m_dev1;
}

SNMapDeviceItem *SNMapConnection::dev2() const
{
	return m_dev2;
}

bool SNMapConnection::isValid() const
{
	if (m_dev1 == NULL || m_dev2 == NULL || m_port1 == 0 || m_port2 == 0)
	{
		return false;
	}
	return true;
}
