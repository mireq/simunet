/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2008 by Miroslav Bendik                                 *
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
#include "SNConfig.h"
#include <QSettings>

/*!
  \class SNConfig
  \brief Trieda ktora sa stara o ukladanie nastaveni.
  \ingroup core
*/

/*!
  Vytvorenie instancie SNConfig.
*/
SNConfig::SNConfig()
{
	m_settings = new QSettings();
}

/*!
  Zrusenie instancie SNConfig, pripadny zapis este neulozenych zmien.
*/
SNConfig::~SNConfig()
{
	delete m_settings;
}

/*!
  Vrati pocet vlakien ktore sa po zapnuti nastartuju.
*/
int SNConfig::threadsCount()
{
	m_settings->beginGroup("preferences");
	m_settings->beginGroup("performance");

	int threads = m_settings->value("threads", QVariant(4)).toInt();

	m_settings->endGroup();
	m_settings->endGroup();
	return threads;
}

/*!
  Nastavenie poctu vlakien.
*/
void SNConfig::setThreadsCount(int threads)
{
	m_settings->beginGroup("preferences");
	m_settings->beginGroup("performance");

	m_settings->setValue("threads", threads);

	m_settings->endGroup();
	m_settings->endGroup();
}


