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
class SNMap;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/
class SNSingleton
{
	public:
		static SNSimulate *getSimulate();
		static void deleteSimulate();
		static SNMap *getMap();
		static void deleteMap();

		template <class T> static T *getDynSettings();
		static void deleteDynSettings();

		static void deleteObjects();
	private:
/*!
  Odkaz na instanciu SNSimulate.
*/
		static SNSimulate *m_simulate;

/*!
  Odkaz na instanciu SNMap.
*/
		static SNMap *m_map;

/*!
  Zoznam odkazov na instancie SNDynamicSettings. Klucom je ID metatypu
  (qMetaTypeId), ktore je unikatne pre kazdu triedu odvodenu od zakladnej
  triedy QObject.
*/
		static QMap<int, SNDynamicSettings*> m_dynSettings;
};

/*!
  Ziskanie referencie na subtriedu dynamickych nastaveni urcenu parametrom T.
*/
template<class T> T *SNSingleton::getDynSettings()
{
	int id = qMetaTypeId<T*>();
	if (!m_dynSettings.contains(id))
	{
		m_dynSettings[id] = new T;
	}
	return static_cast<T*>(m_dynSettings[id]);
}


#endif
