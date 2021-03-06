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
#include "SNColorSelectWidget.h"
#include "SNColorSelectWidget_p.h"

#include <QHBoxLayout>
#include <QPushButton>

#include <QColorDialog>

/*!
  \class SNColorSelectWidget
  \brief Prvok na vyber farieb.
  \ingroup widgets

  Tento prvok sluzi na vyber farby a zobrazenie nahladu na farbu.
 */

/*!
  Vytvorenie noveho prvku na vyber farby.
*/
SNColorSelectWidget::SNColorSelectWidget(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	setLayout(layout);

	m_colorPreview = new SNColorPreviewWidget_Private::SNColorPreviewWidget();
	m_selectButton = new QPushButton(tr("Select ..."));

	layout->addWidget(m_colorPreview);
	layout->addWidget(m_selectButton);

	connect(m_selectButton, SIGNAL(clicked()), SLOT(selectColor()));
}

/*!
  Zrusenie prvku na vyber farby.
*/
SNColorSelectWidget::~SNColorSelectWidget()
{
}

/*!
  Zobrazenie dialogu vyberu farby.
*/
void SNColorSelectWidget::selectColor()
{
	QColor color = QColorDialog::getColor(m_colorPreview->color(), this);
	if (color.isValid())
	{
		setColor(color);
	}
}

/*!
  Vrati aktualne nastavenu farbu.
*/
QColor SNColorSelectWidget::color() const
{
	return m_colorPreview->color();
}

/*!
  Nastavenie na farbu urcenu argumentom \a color.
*/
void SNColorSelectWidget::setColor(const QColor &color)
{
	if (color != m_colorPreview->color())
	{
		m_colorPreview->setColor(color);
		emit colorChanged(color);
	}
}

/* ------------------------------------------------------------------ */

/*!
  \fn void SNColorSelectWidget::colorChanged(const QColor &color)

  Signal sa emituje vtedy, ked uzivatel zmeni farbu, alebo farba sa zmeni
  volanim setColor.
 */

/* ------------------------------------------------------------------ */


namespace SNColorPreviewWidget_Private
{

SNColorPreviewWidget::SNColorPreviewWidget(QWidget *parent)
	: QWidget(parent), m_color(Qt::white)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAutoFillBackground(true);
	updateColor();
}

SNColorPreviewWidget::~SNColorPreviewWidget()
{
}

QSize SNColorPreviewWidget::sizeHint() const
{
	QFontMetrics fm(font());
	return QSize(fm.height(), fm.height());
}

void SNColorPreviewWidget::setColor(const QColor &color)
{
	m_color = color;
	updateColor();
}

const QColor &SNColorPreviewWidget::color() const
{
	return m_color;
}

void SNColorPreviewWidget::updateColor()
{
	QPalette p = palette();
	p.setBrush(QPalette::Background, m_color);
	setPalette(p);
}

}
