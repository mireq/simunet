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
#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

#include <QWidget>
#include "SNIcon.h"

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
 @brief Konfiguracny panel.
 Od tejto triedy sa odvadzaju jednotlive konfiguracne panely.
*/
class SNConfigPanel : public QWidget
{
		Q_OBJECT
	public:
		SNConfigPanel(QWidget* parent = 0);
		~SNConfigPanel();

/*!
 \brief Nazov panelu zobrazujuci sa v zozname panelov.
*/
		virtual QString panelName() const = 0;

/*!
 \brief Ikona konfiguracneho panelu.
 */
		virtual SNIcon icon() const = 0;

/*!
 \brief Ulozenie zmien urobenych v konfiguracnom paneli.
 */
		virtual void saveChanges() = 0;

/*!
 \brief Tato funkcia vracia true ak uzivatel neklikol na cancel pri prechode na iny panel.
*/
		virtual bool panelChanged() {return true;};
		virtual bool panelSelected() {return true;};

	signals:
		void info(QString msg, QString comment = QString(), int msecs = 0);
		void warning(QString msg, QString comment = QString(), int msecs = 0);
		void error(QString msg, QString comment = QString(), int msecs = 0);
		void changed(bool);

};

#endif
