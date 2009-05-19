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
#ifndef SNDYNAMICSETTINGS_H
#define SNDYNAMICSETTINGS_H

#include <QObject>
#include <QMetaType>

#include <QFont>

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/
class SNDynamicSettings : public QObject
{
	Q_OBJECT
	public:
		SNDynamicSettings(QObject *parent = 0);
		virtual ~SNDynamicSettings() = 0;
	signals:
		void settingsChanged();

};

Q_DECLARE_METATYPE(SNDynamicSettings *);

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
 */
class SNGuiSettings : public SNDynamicSettings
{
	Q_OBJECT
	public:
/*!
  Typy fontov ktore mozu byt nastavene v aplikacii.
*/
		enum FontType
		{
			APP_FONT = 0, /*!< Aplikacny font platny pre celu aplikaciu. */
			TERM_FONT     /*!< Font pouzivany v integrovanom emulatore terminalu */
		};

		SNGuiSettings(QObject *parent = 0);
		~SNGuiSettings();

		QFont guiFont(FontType type) const;
		void setGuiFont(const QFont &font, FontType type);
		QFont defaultFont(FontType type) const;
		static const int NumFonts;

		bool antialiasing() const;
		void setAntialiasing(bool antialiasing);

	signals:
		void termFontChanged(const QFont &font);
		void antialiasingChanged(bool antialiasing);

	private:
		QFont **m_defaultFont;
		QFont **m_font;
		static const char *m_fontNames[];
		bool m_antialiasing;

};

Q_DECLARE_METATYPE(SNGuiSettings *);

#endif
