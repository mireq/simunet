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
#ifndef SNABSTRACTDEVICESSCENE_H
#define SNABSTRACTDEVICESSCENE_H

#include <QObject>
#include <QSet>
#include "sntypes.h"

class SNMap;
class SNMapDeviceItem;

/**
 @author Miroslav Bendik
*/
class SNAbstractDevicesScene
{
	public:
		SNAbstractDevicesScene();
		~SNAbstractDevicesScene();
		virtual void setMap(SNMap *map);
		SNMap *map() const;
		virtual void addDevice(SNMapDeviceItem *item);
		virtual void removeDevice(SNMapDeviceItem *item);
/*!
  Znovunacitanie informacii o zariadeni.
*/
		virtual void updateDevice(SNMapDeviceItem *item) = 0;

/*!
  Pridanie fyzickeho portu zariadeniu.
*/
		virtual void addHwPort(SNMapDeviceItem *item, port_num hwPort) = 0;
/*!
  Odstranenie fyzickeho portu zo zariadenia.
*/
		virtual void removeHwPort(SNMapDeviceItem *item, port_num hwPort) = 0;

	protected:
/*!
  Odkaz na mapu, ktoru ovlada pohlad.
*/
		SNMap *m_map;

/*!
  Zoznam zariadeni v pohlade.
*/
		QSet<SNMapDeviceItem *> m_devices;
};

#endif
