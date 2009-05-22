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
#include "SNDynamicSettings.h"

#include <QApplication>
#include <QFontInfo>
#include <QSettings>
#include <QPalette>

/* ----------------------------------------------------------------------- */

/*!
  \class SNDynamicSettings
  \brief Abstraktna trieda urcena na modifikaciu dynamickych nastaveni.
  \ingroup core

  Pod pojmom dynamicke nastavenia sa mysli nastavenia, ktore platia pre celu
  aplikaciu a po ich zmene sa okamzite prejavia zmeny v celej aplikacii
  (napr. nastavenie vzhladu SNGuiSettings).
*/

/*!
  Vytvorenie dynamickych nastaveni.
*/
SNDynamicSettings::SNDynamicSettings(QObject *parent)
	: QObject(parent)
{
}

/*!
  Zrusenie dynamickych nastaveni.
*/
SNDynamicSettings::~SNDynamicSettings()
{
}

/* ----------------------------------------------------------------------- */

/*!
  Pocet typov prvkov pre ktore je mozne vyberat fonty.
*/
const int SNGuiSettings::NumFonts = 2;
const int SNGuiSettings::NumColors = 2;


/*!
  Mena typov prvkov (tak ako sa ukladaju do suboru).
*/
const char *SNGuiSettings::m_fontNames[] = {
	"ApplicationFont",
	"TerminalFont"
};

const char *SNGuiSettings::m_colorNames[] = {
	"BackgroundColor",
	"GridColor"
};


/*!
  \class SNGuiSettings
  \brief Nastavenie vzhladu aplikacie.
  \ingroup core
*/

/*!
  Vytvorenie nastaveni vzhladu aplikacie.

  \warning Instancia tejto triedy by sa mala vytvarat len pomocou SNSingleton.
*/
SNGuiSettings::SNGuiSettings(QObject *parent)
	: SNDynamicSettings(parent)
{
	m_defaultFont = new QFont *[NumFonts];
	m_font        = new QFont *[NumFonts];
	m_colors      = new QColor *[NumColors];
	for (int i  = 0; i < NumFonts; ++i)
	{
		m_defaultFont[i] = NULL;
		m_font[i]        = NULL;
	}
	for (int i  = 0; i < NumColors; ++i)
	{
		m_colors[i] = NULL;
	}
	QSettings settings;
	settings.beginGroup("gui");
	m_antialiasing =  settings.value("antialiasing", QVariant(true)).toBool();
	for (int i  = 0; i < NumColors; ++i)
	{
		QVariant color = settings.value(m_colorNames[i], QVariant());
		if (color.isValid())
		{
			setColor(color.value<QColor>(), ColorGroup(i));
		}
	}
	settings.endGroup();
}

/*!
  Zrusenie nastavenia vzhladu aplikacie.
*/
SNGuiSettings::~ SNGuiSettings()
{
	QSettings settings;
	settings.beginGroup("gui");
	for (int i = 0; i < NumFonts; ++i)
	{
		if (m_font[i] != NULL && m_defaultFont[i] != NULL && *m_font[i] != *m_defaultFont[i])
		{
			settings.setValue(m_fontNames[i], QVariant(*m_font[i]));
		}
		else
		{
			settings.remove(m_fontNames[i]);
		}
	}

	for (int i  = 0; i < NumColors; ++i)
	{
		if (m_colors[i] == NULL)
		{
			settings.remove(m_colorNames[i]);
		}
		else
		{
			settings.setValue(m_colorNames[i], QVariant(*m_colors[i]));
		}
	}
	settings.setValue("antialiasing", QVariant(m_antialiasing));
	settings.endGroup();
	delete m_defaultFont;
	delete m_font;
}

/*!
  Zistenie fontu pre vybrany typ prvku specifikovany atributom \a type.

  \sa FontType, setGuiFont
*/
QFont SNGuiSettings::guiFont(FontType type) const
{
	if (m_font[type] == NULL)
	{
		if (m_defaultFont[type] == NULL)
		{
			m_defaultFont[type] = new QFont(defaultFont(type));
		}

		QSettings settings;
		settings.beginGroup("gui");
		QVariant f = settings.value(m_fontNames[type]);
		settings.endGroup();

		if (f.isValid())
		{
			m_font[type] = new QFont(f.value<QFont>());
		}
		else
		{
			m_font[type] = new QFont(*m_defaultFont[type]);
		}
		return QFont(*m_font[type]);
	}
	else
	{
		return QFont(*m_font[type]);
	}
}

/*!
  Nastavenie fontu pre typ prvku specifikovany atributom \a type.

  \sa guiFont, FontType
*/
void SNGuiSettings::setGuiFont(const QFont &font, FontType type)
{
	QFont oldFont = guiFont(type);
	if (oldFont == font)
	{
		return;
	}

	if (m_font[type] != NULL)
	{
		delete m_font[type];
		m_font[type] = NULL;
	}
	if (font != *m_defaultFont[type])
	{
		m_font[type] = new QFont(font);
	}

	if (type == APP_FONT)
	{
		qApp->setFont(font, "QWidget");
	}
	else if (type == TERM_FONT)
	{
		emit termFontChanged(font);
	}
	emit settingsChanged();
}

/*!
  Zistenie standardneho fontu pre typ prvku specifikovany atributom \a type.
*/
QFont SNGuiSettings::defaultFont(FontType type) const
{
	QFont font = qApp->font();
	switch (type)
	{
		case APP_FONT:
			break;
		case TERM_FONT:
			font.setFamily(QString());
			font.setStyleHint(QFont::TypeWriter);
			font.setFamily(QFontInfo(font).family());
			font.setStyleHint(QFont::AnyStyle);
			break;
	}
	return font;
}

bool SNGuiSettings::antialiasing() const
{
	return m_antialiasing;
}

void SNGuiSettings::setAntialiasing(bool antialiasing)
{
	if (antialiasing != m_antialiasing)
	{
		m_antialiasing = antialiasing;
		emit antialiasingChanged(antialiasing);
	}
}

QColor SNGuiSettings::color(ColorGroup group) const
{
	QPalette palette;
	QColor hgColor = palette.color(QPalette::Highlight);
	QColor bgColor = palette.color(QPalette::Base);
	switch (group)
	{
		case GridColor:
			if (m_colors[group] == NULL)
			{
				return QColor((bgColor.red() * 3 + hgColor.red()) / 4, (bgColor.green() * 3 + hgColor.green()) / 4, (bgColor.blue() * 3 + hgColor.blue()) / 4);
			}
			else
			{
				return *m_colors[group];
			}
		case BgColor:
			if (m_colors[group] == NULL)
			{
				return palette.color(QPalette::Base);
			}
			else
			{
				return *m_colors[group];
			}
		default:
			return QColor();
	}
}

void SNGuiSettings::setColor(const QColor &color, ColorGroup group)
{
	if (m_colors[group] == NULL)
	{
		m_colors[group] = new QColor(color);
		emit colorChanged(color, group);
	}
	else
	{
		if (*m_colors[group] == color)
		{
			return;
		}
		else
		{
			delete m_colors[group];
			m_colors[group] = new QColor(color);
			emit colorChanged(color, group);
		}
	}
}

void SNGuiSettings::resetColor(ColorGroup group)
{
	if (m_colors[group] != NULL)
	{
		delete m_colors[group];
		m_colors[group] = NULL;
		emit colorChanged(color(group), group);
	}
}

bool SNGuiSettings::colorIsDefault(ColorGroup group)
{
	if (m_colors[group] == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ----------------------------------------------------------------

/*!
  \fn SNDynamicSettings::settingsChanged()

  Tento signal je emitovany pri zmene nastaveni v sbutriede SNDynamicSettings.
*/

// ----------------------------------------------------------------

/*!
  \fn SNGuiSettings::termFontChanged(const QFont &font)

  Tento signal sa vyvola len pri zmene terminaloveho fontu.

  \sa setGuiFont, FontType
*/


