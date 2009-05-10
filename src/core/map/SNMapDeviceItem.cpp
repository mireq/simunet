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
#include "SNMapDeviceItem.h"

#include "core/SNAccessors.h"
#include "core/SNDevice.h"
#include "core/SNSimulate.h"

/*!
  \class SNMapDeviceItem
  \brief Reprezentacia zariadenia na mape.
  \ingroup map

  Tato treieda uchovava rozsirujuce informacie o zariadeni. Zariadenie v
  simulacnom jadre je len trieda prepajajuca pythonovske zariadenie s C++.
  Jediny atribut ktory ma je unikatne ID. Tato trieda pridava informacie o
  polohe zariadenia.

  \sa SNDevice
*/

/*!
  Vytvorenie noveho zariadenia so svojim unikatnym ID zariadenia.
*/
SNMapDeviceItem::SNMapDeviceItem(uint32_t deviceId)
	: m_device(NULL)
{
	m_deviceId = deviceId;
}

/*!
  Zrusenie zariadenia.
*/
SNMapDeviceItem::~SNMapDeviceItem()
{
}

/*!
  Nastavenie ineho ID zariadenia.
*/
void SNMapDeviceItem::setDeviceId(uint32_t deviceId)
{
	m_deviceId = deviceId;
	m_device = NULL;
}

/*!
  Zistenie ID zariadenia.
*/
uint32_t SNMapDeviceItem::deviceId() const
{
	return m_deviceId;
}

SNDevice *SNMapDeviceItem::device()
{
	if (m_device == NULL)
	{
		m_device = SNSingleton::getSimulate()->device(m_deviceId);
	}
	return m_device;
}

std::string SNMapDeviceItem::name() const
{
	return m_name;
}

void SNMapDeviceItem::setName(std::string name)
{
	m_name = name;
}


