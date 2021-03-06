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

#include "SNFontSelect.h"

#include "core/SNGuiSettings.h"

#include <QColor>


class QPushButton;
class SNColorSelectWidget;

namespace CfgApperance_Private
{

class FontSelect : public SNFontSelect
{
	Q_OBJECT
	public:
		FontSelect(const QFont &font, SNGuiSettings::FontType type, QWidget *parent = 0);
		~FontSelect();
		SNGuiSettings::FontType type() const;
	signals:
		void fontChanged(const QFont &font, SNGuiSettings::FontType);
	private slots:
		void emitFontChanged(const QFont &font);
	private:
		SNGuiSettings::FontType m_type;
};

class ColorSelectHandler: public QObject
{
	Q_OBJECT
	public:
		ColorSelectHandler(SNColorSelectWidget *widget, QPushButton *resetBtn, SNGuiSettings::ColorGroup group, QObject *parent = 0);
		~ColorSelectHandler();
	private slots:
		void newColor(const QColor &color);
		void resetTriggered();
	signals:
		void colorChanged(const QColor &color, SNGuiSettings::ColorGroup group);
		void reset(SNGuiSettings::ColorGroup group);
	private:
		SNColorSelectWidget *m_colorSelect;
		SNGuiSettings::ColorGroup m_group;
		QPushButton *m_resetBtn;
};

}
