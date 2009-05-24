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
#ifndef CFGAPPERANCE_H
#define CFGAPPERANCE_H

#include "SNConfigPanel.h"

#include "core/SNGuiSettings.h"

class QGridLayout;
class QPushButton;
class QSignalMapper;

namespace CfgApperance_Private
{
class FontSelect;
}

class SNGUiSettings;
class SNColorSelectWidget;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>SNColorSelectWidget
*/
class CfgApperance : public SNConfigPanel
{
		Q_OBJECT
	public:
		CfgApperance(QWidget* parent = 0);

		~CfgApperance();

		virtual QString panelName() const;
		virtual SNIcon icon() const;
		virtual void saveChanges();
		virtual bool settingsChanged();

	private slots:
		void fontChanged(const QFont &font, SNGuiSettings::FontType type);
		void fontReset(int type);
		void on_antialiasing_stateChanged(int state);
		void colorChanged(const QColor &color, SNGuiSettings::ColorGroup group);
		void resetColor(SNGuiSettings::ColorGroup group);

	private:
		void addFontSelectRow(const QString &label, CfgApperance_Private::FontSelect *fontSelect);
		void addColorSelectRow(const QString &label, SNColorSelectWidget *colorSelect, SNGuiSettings::ColorGroup group);

	private:
/*!
  Rozlozenie, v ktorom sa nachadzaju dialogy pre vyber fontu.
*/
		QGridLayout *m_fontsSelectLayout;
/*!
  Rozlozenie, v ktorom su polozky pre nastavenie vzhladu aplikacie.
*/
		QGridLayout *m_apperanceLayout;
/*!
  Widget pre vyber fontu aplikacie.
*/
		CfgApperance_Private::FontSelect *m_appFontSelect;

/*!
  Widget pre vyber fontu emulatora terminalu.
 */
		CfgApperance_Private::FontSelect *m_termFontSelect;

/*!
  Tlacidla na obnovenie povodnych nastaveni fontu.
*/
		QPushButton **m_fontResetButtons;

/*!
  Prvky na nastavenie farieb.
*/
		SNColorSelectWidget **m_colorSelectWidgets;

/*!
  Priznak zmeny fontu (indexom pola je typ fontu).
*/
		bool *m_changed;

/*!
  Mapovanie resetovacich tlacidiel na sloty.
*/
		QSignalMapper *m_mapper;

};

#endif
