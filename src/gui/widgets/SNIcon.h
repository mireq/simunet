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
#ifndef SNICON_H
#define SNICON_H

#include <QIcon>

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
 @brief Trieda pouzitelna ako nahrada QIcon (kvoli kompatibilite s KDE).
*/
class SNIcon : public QIcon
{
	public:
/*!
 \brief Konstruktor vytvarajuci ikonu.
 \param QString Nazov ikony
 Nazov ikony sa zadava bez uvodneho : a pripony (.png). Nazvy su
 kompatibilne s KDE4. V pripade, ze sa ziadana ikona nenajde nacita sa
 unknown.png.
*/
		SNIcon(const QString& name, bool showUnknownIcon = true);
		virtual ~SNIcon();
	private:
		static const int m_size[7];
};

#endif
