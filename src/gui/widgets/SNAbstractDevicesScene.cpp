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
#include "SNAbstractDevicesScene.h"

#include "core/map/SNMap.h"
#include "core/map/SNMapDeviceItem.h"

#include <QDebug>

SNAbstractDevicesScene::SNAbstractDevicesScene(QObject *parent)
		: QObject(parent), m_map(NULL)
{
}


SNAbstractDevicesScene::~SNAbstractDevicesScene()
{
}

void SNAbstractDevicesScene::setMap(SNMap *map)
{
	m_map = map;
}

SNMap *SNAbstractDevicesScene::map() const
{
	return m_map;
}

void SNAbstractDevicesScene::addDevice(SNMapDeviceItem *item)
{
	if (!m_devices.contains(item))
	{
		m_devices.insert(item);
	}
}

void SNAbstractDevicesScene::removeDevice(SNMapDeviceItem *item)
{
	if (m_devices.contains(item))
	{
		m_devices.remove(item);
	}
}


