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
#ifndef CFGPERFORMANCE_H
#define CFGPERFORMANCE_H

#include <QWidget>
#include "SNIcon.h"
#include "SNConfigPanel.h"

class QSpinBox;

/**
 @author Miroslav Bendik
*/
class CfgPerformance : public SNConfigPanel
{
		Q_OBJECT
	public:
		CfgPerformance(QWidget* parent = 0);
		~CfgPerformance();
		virtual QString panelName() const;
		virtual SNIcon icon() const;
		virtual bool panelSelected();
		virtual void saveChanges();
		virtual bool settingsChanged();
		virtual void dropChanges();

	private:
		void readConfig();

/*!
  Nastavenie poctu vlakien.
*/
		QSpinBox *m_threadsCount;
/*!
  Pocet vlakien.
*/
		int m_sThreadsCount;

	private slots:
		void updateThreads(int newValue);

};

#endif
