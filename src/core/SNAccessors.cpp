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
#include "SNAccessors.h"
#include "SNSimulate.h"
#include "SNConfig.h"

#include "SNDynamicSettings.h"


SNSimulate *SNSingleton::m_simulate = NULL;
QMap<int, SNDynamicSettings*> SNSingleton::m_dynSettings = QMap<int, SNDynamicSettings*>();

/*!
  \class SNSingleton
  \brief Zaistenie globalneho pristupu k triedam, ktore mozu mat 1 instanciu.

  \ingroup core
*/


/*!
  Ziskanie smernika na instanciu SNSimulate.

 Vytvori novu instanciu SNSimulate (ak este nebola vytvorena) a vrati smernik
 na tuto instanciu.

 \sa SNSimulate
*/
SNSimulate *SNSingleton::getSimulate()
{
	if (m_simulate == NULL)
	{
		SNConfig config;
		m_simulate = new SNSimulate(config.threadsCount());
	}
	return m_simulate;
}

/*!
 Zrusenie instancie SNSimulate.

 Ak instancia nebola vytvorena neurobi tato funkcia nic.

 \warning Tato instancia sa nezrusi sama, je nutne volat destruktor rucne.
*/
void SNSingleton::deleteSimulate()
{
	if (m_simulate != NULL)
	{
		delete m_simulate;
	}
	m_simulate = NULL;
}

void SNSingleton::deleteDynSettings()
{
	QMap<int, SNDynamicSettings*>::iterator it;
	for (it = m_dynSettings.begin(); it != m_dynSettings.end(); ++it)
	{
		delete (*it);
	}
}

void SNSingleton::deleteObjects()
{
	deleteSimulate();
	deleteDynSettings();
}




