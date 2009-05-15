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
#include "SNHwPort.h"

SNHwPort::SNHwPort(uint32_t devId, port_num hwPort, SNDevice *device)
	: m_devId(devId), m_hwPort(hwPort), m_buddy(0), m_device(device)
{
}

SNHwPort::SNHwPort(const SNHwPort &other)
{
	m_devId = other.m_devId;
	m_hwPort = other.m_hwPort;
	setBuddy(*(other.m_buddy));
	m_device = other.m_device;
}

SNHwPort::~SNHwPort()
{
	if (m_buddy != 0)
	{
		delete m_buddy;
	}
}

void SNHwPort::setBuddy(const SNHwPort &buddy)
{
	m_buddy = new SNHwPort(buddy.m_devId, buddy.m_hwPort, buddy.m_device);
}

void SNHwPort::unsetBuddy()
{
	delete m_buddy;
	m_buddy = 0;
}

void SNHwPort::setDevice(SNDevice *device)
{
	m_device = device;
}

SNHwPort *SNHwPort::buddy() const
{
	return m_buddy;
}

uint32_t SNHwPort::devId() const
{
	return m_devId;
}

port_num SNHwPort::hwPort() const
{
	return m_hwPort;
}

SNDevice *SNHwPort::device() const
{
	return m_device;
}


bool SNHwPort::operator <(const SNHwPort &other) const
{
	if (this->m_devId == other.m_devId)
	{
		return this->m_hwPort < other.m_hwPort;
	}
	else
	{
		return this->m_devId < other.m_devId;
	}
}

bool SNHwPort::operator >(const SNHwPort &other) const
{
	if (this->m_devId == other.m_devId)
	{
		return this->m_hwPort > other.m_hwPort;
	}
	else
	{
		return this->m_devId > other.m_devId;
	}
}

bool SNHwPort::operator ==(const SNHwPort &other) const
{
	return (this->m_devId == other.m_devId && this->m_hwPort == other.m_hwPort);
}





