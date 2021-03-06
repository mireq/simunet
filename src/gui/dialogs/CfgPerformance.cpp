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
#include "CfgPerformance.h"
#include "core/SNConfig.h"

#include <QFormLayout>
#include <QMessageBox>
#include <QSpinBox>
#include <QSettings>

/*!
  \class CfgPerformance
  \brief Konfiguracia vykonu aplikacie
  \ingroup dialogs
*/

/*!
  Vytvorenie panelu s nastavenim vykonu.
*/
CfgPerformance::CfgPerformance(QWidget* parent): SNConfigPanel(parent)
{
	QFormLayout *layout = new QFormLayout(this);
	m_threadsCount = new QSpinBox();
	m_threadsCount->setMinimum(1);
	m_threadsCount->setMaximum(31);
	layout->addRow(tr("&Number of threads"), m_threadsCount);
	connect(m_threadsCount, SIGNAL(valueChanged(int)), SLOT(updateThreads(int)));
}

/*!
  Zrusenie panelu.
*/
CfgPerformance::~CfgPerformance()
{
}

/*!
  \reimp
*/
QString CfgPerformance::panelName() const
{
	return QString(tr("Performance"));
}

/*!
  \reimp
*/
SNIcon CfgPerformance::icon() const
{
	return SNIcon("preferences-system-performance");
}

/*!
  \reimp
*/
bool CfgPerformance::panelSelected()
{
	readConfig();
	emit changed(false);
	return false;
}

/*!
  \reimp
*/
void CfgPerformance::saveChanges()
{
	SNConfig config;
	int val = m_threadsCount->value();
	config.setThreadsCount(val);
	m_sThreadsCount = val;
	emit changed(settingsChanged());
}

/*!
  Nacitanie konfiguracie.
*/
void CfgPerformance::readConfig()
{
	SNConfig config;
	m_sThreadsCount = config.threadsCount();
	m_threadsCount->setValue(m_sThreadsCount);
}

/*!
  \reimp
*/
bool CfgPerformance::settingsChanged()
{
	if (m_sThreadsCount != m_threadsCount->value())
	{
		return true;
	}
	return false;
}

/*!
  \reimp
*/
void CfgPerformance::dropChanges()
{
	m_threadsCount->setValue(m_sThreadsCount);
}

/*!
  Vypis varovania o nutnosti restartu aplikacie pri zmene poctu vlakien.
*/
void CfgPerformance::updateThreads(int newValue)
{
	emit changed(settingsChanged());
	if (m_sThreadsCount != newValue)
	{
		emit warning(tr("The number of threads will change after restart."), QString(), 5000);
	}
}


