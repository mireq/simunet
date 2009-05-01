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
#include "SNPoint3f.h"

SNPoint3f::SNPoint3f()
	:m_x(0.0), m_y(0.0), m_z(0.0)
{
}

SNPoint3f::SNPoint3f(float x, float y, float z)
	:m_x(x), m_y(y), m_z(z)
{
}

SNPoint3f::~ SNPoint3f()
{
}

void SNPoint3f::setX(float x)
{
	m_x = x;
}

void SNPoint3f::setY(float y)
{
	m_y = y;
}

void SNPoint3f::setZ(float z)
{
	m_z = z;
}

void SNPoint3f::setCoord(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

float SNPoint3f::x() const
{
	return m_x;
}

float SNPoint3f::y() const
{
	return m_y;
}

float SNPoint3f::z() const
{
	return m_z;
}

