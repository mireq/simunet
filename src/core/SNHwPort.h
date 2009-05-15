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
#ifndef SNHWPORT_H
#define SNHWPORT_H

#include "sntypes.h"

class SNDevice;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/
class SNHwPort
{
	public:
		SNHwPort(uint32_t devId, port_num hwPort, SNDevice *device = 0);
		SNHwPort(const SNHwPort &other);
		~SNHwPort();
		void setBuddy(const SNHwPort &buddy);
		void unsetBuddy();
		void setDevice(SNDevice *device);
		SNHwPort *buddy() const;
		uint32_t devId() const;
		port_num hwPort() const;
		SNDevice *device() const;
		bool operator <(const SNHwPort &other) const;
		bool operator >(const SNHwPort &other) const;
		bool operator ==(const SNHwPort &other) const;
	private:
		uint32_t m_devId;
		uint32_t m_hwPort;
		SNHwPort *m_buddy;
		SNDevice *m_device;
};

#endif
