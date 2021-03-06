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
#ifndef SNMAPDEVICEITEM_H
#define SNMAPDEVICEITEM_H

#include "SNMapItem.h"
#include "sntypes.h"
#include <string>

class SNDevice;

/**
 @author Miroslav Bendik
*/
class SNMapDeviceItem: public SNMapItem
{
	public:
		SNMapDeviceItem(uint32_t deviceId = 0);
		~SNMapDeviceItem();

		uint32_t deviceId() const;
		void setDeviceId(uint32_t deviceId);

		SNDevice *device();
		std::string name() const;
		void setName(std::string name);

	private:
/*!
  Unikatne ID zaraidenia.
*/
		uint32_t m_deviceId;

/*!
  Cachovane zariadenie.
*/
		SNDevice *m_device;
		std::string m_name;

};

#endif
