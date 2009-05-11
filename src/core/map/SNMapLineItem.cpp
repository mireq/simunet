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
#include "SNMapLineItem.h"
#include "SNMapControlPointItem.h"
#include "core/SNExceptions.h"
#include "SNMapDeviceItem.h"

#include <QDebug>

using namespace std;

SNMapLineItem::SNMapLineItem()
{
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
	qDebug()<<device->deviceId()<<port;
	addControlPoint(new SNMapControlPointItem(SNPoint3f(x, y, z)), pos);
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
}

void SNMapLineItem::removeControlPoint(std::vector<SNMapControlPointItem *>::size_type pos)
{
	if (pos >= m_controlPoints.size())
	{
		throw SNoutOfRangeException();
	}

	std::vector<SNMapControlPointItem *>::iterator iter = m_controlPoints.begin() + pos;
	SNMapControlPointItem *point = *iter;
	m_controlPoints.erase(iter);
	delete point;
}





