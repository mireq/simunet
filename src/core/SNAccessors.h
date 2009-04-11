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
#ifndef SNACCESSORS_H
#define SNACCESSORS_H

#include <QMap>
#include <QMetaType>

class SNSimulate;
class SNDynamicSettings;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/
class SNSingleton
{
	public:
		static SNSimulate *getSimulate();
		static void deleteSimulate();

		template <class T> static T *getDynSettings();
		static void deleteDynSettings();

		static void deleteObjects();
	private:
		static SNSimulate *m_simulate;
		static QMap<int, SNDynamicSettings*> m_dynSettings;
};

template<class T> T *SNSingleton::getDynSettings()
{
	int id = qMetaTypeId<T*>();
	if (!m_dynSettings.contains(id))
	{
		m_dynSettings[id] = new T;
	}
	return m_dynSettings[id];
}


#endif
