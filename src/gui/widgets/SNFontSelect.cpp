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

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFontDialog>

/*!
  \class SNFontSelect
  \brief Widget na vyber fontu.
  \ingroup widgets

  Tento widget sa sklada z labelu ktory obsahuje meno fontu a jeho nahlad
  a tlacitka na vyber fontu.
*/

/*!
  Vytvorenie widgetu na vyber fontov.

  Na zaciatku bude mat standardny font urceny atributom \a font.
*/
SNFontSelect::SNFontSelect(const QFont &font, QWidget *parent)
		: QWidget(parent)
{
	m_font = font;

	QHBoxLayout *layout = new QHBoxLayout(this);
	m_label = new QLabel;
	m_label->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	QPushButton *button = new QPushButton(tr("Select ..."));

	layout->addWidget(m_label);
	layout->addWidget(button);

	setSelectedFont(font);

	connect(button, SIGNAL(clicked()), SLOT(selectFont()));
}

/*!
  Zrusenie widgetu na vyber fontu.
*/
SNFontSelect::~SNFontSelect()
{
}

/*!
  Ziskanie aktuane vybraneho fontu.
*/
QFont SNFontSelect::selectedFont() const
{
	return m_font;
}

/*!
  Nastavenie vybraneho fontu na vlastny font urceny atributom \a font.
*/
void SNFontSelect::setSelectedFont(const QFont &font)
{
	bool changed = false;
	if (m_font != font)
	{
		changed = true;
	}

	m_font = font;
	QFontInfo fontInfo(font);
	m_label->setText(QString("%1 %2").arg(fontInfo.family()).arg(fontInfo.pointSize()));
	m_label->setFont(font);

	if (changed)
	{
		emit fontChanged(font);
	}
}

/*!
  Zobrazenie dialogu vyberu fontov a nastavenie fontu na font, ktory vybral
  uzivatel.
*/
void SNFontSelect::selectFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, selectedFont(), this);
	if (ok)
	{
		setSelectedFont(font);
	}
}

/* ----------------------------------------------------------------------- */

/*!
  \fn SNFontSelect::fontChanged(const QFont &font)

  Tento signal sa emituje vzdy, ked sa zmeni vybrany font.
*/
