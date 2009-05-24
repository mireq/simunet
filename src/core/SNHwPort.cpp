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

/*!
  \class SNHwPort
  \brief Tato trieda ma informacie o porte zariadenia a susednom porte.
  \ingroup core
 */

/*!
  Vytvorenie fyzickeho potru zariadenia.

  \param devId ID zariadenia.
  \param hwPort Cislo fyzickeho portu.
  \param device Zariadenie, ktoremu patri port.
*/
SNHwPort::SNHwPort(uint32_t devId, port_num hwPort, SNDevice *device)
	: m_devId(devId), m_hwPort(hwPort), m_buddy(0), m_device(device)
{
}

/*!
  Vytvorenie kopie portu.
*/
SNHwPort::SNHwPort(const SNHwPort &other)
{
	m_devId = other.m_devId;
	m_hwPort = other.m_hwPort;
	m_buddy = new SNHwPort(other.m_devId, other.m_hwPort, other.m_device);
	m_device = other.m_device;
}

/*!
  Zrusenie portu.
*/
SNHwPort::~SNHwPort()
{
	if (m_buddy != 0)
	{
		delete m_buddy;
	}
}

/*!
  Nastavenie susedneho portu, s ktorym je tento port spojeny.

  \sa buddy, unsetBuddy
*/
void SNHwPort::setBuddy(const SNHwPort &buddy)
{
	if (m_buddy != 0)
	{
		delete m_buddy;
		m_buddy = 0;
	}
	m_buddy = new SNHwPort(buddy.m_devId, buddy.m_hwPort, buddy.m_device);
}

/*!
  Rozpojenie dvoch portov.

  \sa setBuddy, buddy
*/
void SNHwPort::unsetBuddy()
{
	delete m_buddy;
	m_buddy = 0;
}

/*!
  Nastavenie zariadenia, ktoremu prislucha port.

  \sa device
*/
void SNHwPort::setDevice(SNDevice *device)
{
	m_device = device;
}

/*!
  Zistenie portu spojeneho s tymto portom. Ak je port odpojeny vrati \e NULL.
*/
SNHwPort *SNHwPort::buddy() const
{
	return m_buddy;
}

/*!
  ID zariadenia, ktoremu patri port.
*/
uint32_t SNHwPort::devId() const
{
	return m_devId;
}

/*!
  Cislo portu na zariadeni.
*/
port_num SNHwPort::hwPort() const
{
	return m_hwPort;
}

/*!
  Vrati zariadenie, ktoremu patri port.

  \sa setDevice
*/
SNDevice *SNHwPort::device() const
{
	return m_device;
}

/*!
  Porovnanie dvoch portov (kvoli moznosti zoradenia v kontajneri \e map).

  Ak je cislo prveho zariadenia mensie nez cislo druheho vrati \e true. Ak je
  vacsie vrati \e false. V pripade rovnakych cisel zariadenia porovnava porty.
*/
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

/*!
  Porovnanie dvoch portov (kvoli moznosti zoradenia v kontajneri \e map).
*/
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

/*!
  Test na to, ci oba porty su tie iste a patria tomu istemu zariadeniu.
*/
bool SNHwPort::operator ==(const SNHwPort &other) const
{
	return (this->m_devId == other.m_devId && this->m_hwPort == other.m_hwPort);
}





