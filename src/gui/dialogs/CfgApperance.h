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
#include "core/SNDynamicSettings.h"

class QGridLayout;
class QPushButton;
class QSignalMapper;

class FontSelect;
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
		void reset(SNGuiSettings::ColorGroup group);

	private:
		void addFontSelectRow(const QString &label, FontSelect *fontSelect);
		void addColorSelectRow(const QString &label, SNColorSelectWidget *colorSelect, SNGuiSettings::ColorGroup group);

		QGridLayout *m_fontsSelectLayout;
		QGridLayout *m_apperanceLayout;
		FontSelect *m_appFontSelect;
		FontSelect *m_termFontSelect;
		QPushButton **m_fontResetButtons;
		SNColorSelectWidget **m_colorSelectWidgets;
		bool *m_changed;
		SNGuiSettings *m_settings;
		QSignalMapper *m_mapper;

};

#endif
