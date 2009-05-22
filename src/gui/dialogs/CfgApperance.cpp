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
#include "CfgApperance.h"
#include "CfgApperance_p.h"

#include "core/SNAccessors.h"

#include "SNColorSelectWidget.h"
#include "SNIcon.h"

#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSignalMapper>
#include <QGroupBox>
#include <QCheckBox>

#include <QDebug>
#include <QApplication>

FontSelect::FontSelect(const QFont &font, SNGuiSettings::FontType type, QWidget *parent)
	: SNFontSelect(font, parent)
{
	m_type = type;
	connect(this, SIGNAL(fontChanged(const QFont &)), SLOT(emitFontChanged(const QFont &)));
}

FontSelect::~FontSelect()
{
}

SNGuiSettings::FontType FontSelect::type() const
{
	return m_type;
}

void FontSelect::emitFontChanged(const QFont &font)
{
	emit fontChanged(font, m_type);
}

/* ----------------------------------------------------------------------- */

/*!
  \class CfgApperance
  \brief Konfiguracia vzhladu aplikacie
  \ingroup dialogs
 */


/*!
  Vytvorenie okna s nastaveniami vzhladu.
*/
CfgApperance::CfgApperance(QWidget* parent): SNConfigPanel(parent)
{
	// fonty
	m_mapper = new QSignalMapper(this);
	connect(m_mapper, SIGNAL(mapped(int)), SLOT(fontReset(int)));
	m_fontResetButtons = new QPushButton *[SNGuiSettings::NumFonts];
	m_changed = new bool[SNGuiSettings::NumFonts];
	m_settings = SNSingleton::getDynSettings<SNGuiSettings>();

	QFont appFont  = m_settings->guiFont(SNGuiSettings::APP_FONT);
	QFont termFont = m_settings->guiFont(SNGuiSettings::TERM_FONT);

	QVBoxLayout *layout = new QVBoxLayout(this);
	QTabWidget *tabs = new QTabWidget();
	layout->addWidget(tabs);

	m_fontsSelectLayout = new QGridLayout;
	m_fontsSelectLayout->setColumnStretch(0, 0);
	m_fontsSelectLayout->setColumnStretch(1, 1);
	m_fontsSelectLayout->setColumnStretch(2, 0);
	QWidget *fontsSelect = new QWidget;
	fontsSelect->setLayout(m_fontsSelectLayout);

	m_appFontSelect = new FontSelect(appFont, SNGuiSettings::APP_FONT);
	m_termFontSelect = new FontSelect(termFont, SNGuiSettings::TERM_FONT);

	addFontSelectRow(tr("Application font"), m_appFontSelect);
	addFontSelectRow(tr("Terminal font"), m_termFontSelect);

	m_fontsSelectLayout->setRowStretch(2, 1);

	// zobrazenie zariadeni
	// device visualization
	m_colorSelectWidgets = new SNColorSelectWidget *[SNGuiSettings::NumColors];
	QWidget *visSettings = new QWidget;
	QGroupBox *performance = new QGroupBox(tr("Performance"));
	QGroupBox *apperance = new QGroupBox(tr("Apperance"));

	QCheckBox *antialiasing = new QCheckBox(tr("Antialiasing"));

	QVBoxLayout *performanceLayout = new QVBoxLayout;
	m_apperanceLayout = new QGridLayout;
	QVBoxLayout *visSettingsLayout = new QVBoxLayout;

	antialiasing->setObjectName("antialiasing");

	performanceLayout->addWidget(antialiasing);

	m_apperanceLayout->setColumnStretch(0, 1);
	m_apperanceLayout->setColumnStretch(1, 0);
	m_apperanceLayout->setColumnStretch(2, 0);

	visSettingsLayout->addWidget(performance);
	visSettingsLayout->addWidget(apperance);
	visSettingsLayout->addStretch(1);

	performance->setLayout(performanceLayout);
	apperance->setLayout(m_apperanceLayout);
	visSettings->setLayout(visSettingsLayout);

	tabs->addTab(fontsSelect, SNIcon("preferences-desktop-font"), tr("&Fonts"));
	tabs->addTab(visSettings, tr("&Device visualisation"));
	tabs->setMovable(true);

	Qt::CheckState checkState;
	if (m_settings->antialiasing())
	{
		checkState = Qt::Checked;
	}
	else
	{
		checkState = Qt::Unchecked;
	}
	antialiasing->setCheckState(checkState);

	//QLabel *gridColorLabel = new QLabel(tr("Grid color"));
	//SNColorSelectWidget *gridColorSelect = new SNColorSelectWidget;
	//gridColorLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	//m_apperanceLayout->addWidget(gridColorLabel, 0, 0);
	//m_apperanceLayout->addWidget(gridColorSelect, 0, 1);
	addColorSelectRow(tr("Background color"), new SNColorSelectWidget, SNGuiSettings::BgColor);
	addColorSelectRow(tr("Grid color"), new SNColorSelectWidget, SNGuiSettings::GridColor);

	QMetaObject::connectSlotsByName(this);
}

/*!
  Zrusenie okna s nastaveniami vzhladu.
*/
CfgApperance::~CfgApperance()
{
	delete m_changed;
	delete m_fontResetButtons;
}

/*!
  \reimp
*/
QString CfgApperance::panelName() const
{
	return tr("Apperance");
}

/*!
  \reimp
*/
SNIcon CfgApperance::icon() const
{
	return SNIcon("preferences-desktop-theme");
}

/*!
  \reimp
*/
void CfgApperance::saveChanges()
{
	if (settingsChanged())
	{
		m_settings->setGuiFont(m_appFontSelect->selectedFont(), SNGuiSettings::APP_FONT);
		m_settings->setGuiFont(m_termFontSelect->selectedFont(), SNGuiSettings::TERM_FONT);
	}
	emit changed(false);
}

/*!
  Aktualizacia prvkov okna pri zmene fontu.
*/
void CfgApperance::fontChanged(const QFont &font, SNGuiSettings::FontType type)
{
	if (m_settings->defaultFont(type) == font)
	{
		m_fontResetButtons[type]->hide();
	}
	else
	{
		m_fontResetButtons[type]->show();
	}

	if (m_settings->guiFont(type) == font)
	{
		m_changed[type] = false;
	}
	else
	{
		m_changed[type] = true;
	}
	emit changed(settingsChanged());
}

/*!
  Reset danaho typu pisma na standardnu hodnotu.
*/
void CfgApperance::fontReset(int type)
{
	SNGuiSettings::FontType t = static_cast<SNGuiSettings::FontType>(type);
	QFont font = m_settings->defaultFont(t);
	switch (type)
	{
		case SNGuiSettings::APP_FONT:
			m_appFontSelect->setSelectedFont(font);
			break;
		case SNGuiSettings::TERM_FONT:
			m_termFontSelect->setSelectedFont(font);
			break;
	}
}

/*!
  Pridanie riadku s vyberom fontu do layoutu.
*/
void CfgApperance::addFontSelectRow(const QString &label, FontSelect *fontSelect)
{
	QPushButton *button = new QPushButton(tr("Reset"));
	QLabel *l = new QLabel(label);

	m_fontResetButtons[fontSelect->type()] = button;
	m_changed[fontSelect->type()] = false;

	if (m_settings->defaultFont(fontSelect->type()) == fontSelect->selectedFont())
	{
		button->hide();
	}
	m_mapper->setMapping(button, fontSelect->type());
	connect(button, SIGNAL(clicked()), m_mapper, SLOT(map()));

	l->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_fontsSelectLayout->addWidget(l);
	m_fontsSelectLayout->addWidget(fontSelect);
	m_fontsSelectLayout->addWidget(button);
	connect(fontSelect, SIGNAL(fontChanged(const QFont &, SNGuiSettings::FontType)), SLOT(fontChanged(const QFont &, SNGuiSettings::FontType)));
}

/*!
  \reimp
*/
bool CfgApperance::settingsChanged()
{
	for (int i = 0; i < SNGuiSettings::NumFonts; ++i)
	{
		if (m_changed[i])
		{
			return true;
		}
	}
	return false;
}

void CfgApperance::on_antialiasing_stateChanged(int state)
{
	if (state == Qt::Checked)
	{
		m_settings->setAntialiasing(true);
	}
	else
	{
		m_settings->setAntialiasing(false);
	}
}

void CfgApperance::addColorSelectRow(const QString &label, SNColorSelectWidget *colorSelect, SNGuiSettings::ColorGroup group)
{
	m_colorSelectWidgets[group] = colorSelect;
	QLabel *l = new QLabel(label);
	QPushButton *reset = new QPushButton(tr("Reset"));
	colorSelect->setColor(m_settings->color(group));
	ColorSelectHandler *h = new ColorSelectHandler(colorSelect, reset, group, this);

	connect(h, SIGNAL(colorChanged(const QColor &, SNGuiSettings::ColorGroup)), SLOT(colorChanged(const QColor &, SNGuiSettings::ColorGroup)));
	connect(h, SIGNAL(reset(SNGuiSettings::ColorGroup)), SLOT(reset(SNGuiSettings::ColorGroup)));

	if (m_settings->colorIsDefault(group))
	{
		reset->setHidden(true);
	}

	m_apperanceLayout->addWidget(l);
	m_apperanceLayout->addWidget(colorSelect);
	m_apperanceLayout->addWidget(reset);
}

void CfgApperance::colorChanged(const QColor &color, SNGuiSettings::ColorGroup group)
{
	m_settings->setColor(color, group);
}


void CfgApperance::reset(SNGuiSettings::ColorGroup group)
{
	m_settings->resetColor(group);
	m_colorSelectWidgets[group]->setColor(m_settings->color(group));
	m_settings->resetColor(group);
}

ColorSelectHandler::ColorSelectHandler(SNColorSelectWidget *widget, QPushButton *resetBtn, SNGuiSettings::ColorGroup group, QObject *parent)
	: QObject(parent)
{
	m_colorSelect = widget;
	m_group = group;
	m_resetBtn = resetBtn;

	connect(widget, SIGNAL(colorChanged(const QColor &)), SLOT(newColor(const QColor &)));
	connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetTriggered()));
}

ColorSelectHandler::~ColorSelectHandler()
{
}

void ColorSelectHandler::newColor(const QColor &color)
{
	m_resetBtn->setHidden(false);
	emit colorChanged(color, m_group);
}

void ColorSelectHandler::resetTriggered()
{
	emit reset(m_group);
	m_resetBtn->setHidden(true);
}
