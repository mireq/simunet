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
#include "SNIcon.h"
#include <QFile>

const int SNIcon::m_size[7] = {16, 22, 32, 48, 64, 128, 0};

/*!
  \class SNIcon
  \brief Trieda pouzitelna ako nahrada QIcon.
  \ingroup widgets

  Pouzitie tejto triedy je o nieco jednoduchsie nez QIcon. Okrem toho
  umoznuje tato trieda jednoduchsiu integraciu do desktopu. Nazvy ikon su
  kompatibilne s KDE 4.

  Ikony sa prehladavaju v adresari :/icons/XxY/name.png. Ikony mozu mat niekolko
  velkosti. Ak ma ikona niekolko velkosti nespomali to beh. K nacitaniu ikony
  dojde az pri poziadavke na vykreslenie (kedy sa uz vie aku pozadujeme velkost).
*/

/*!
  Vytvorenie novej ikony.

  \param name Nazov ikony. Podla nazvu sa bude vyhladavat subor.
  \param showUnknownIcon V pripade, ze je tento parameter true a ikona sa
    nenajde bude nahradena neznamou ikonou.
*/
SNIcon::SNIcon(const QString &name, bool showUnknownIcon)
{
	int sizeIndex = 0;
	int filesFound = 0;
	int size;
	QString prefix = ":icons/";
	QString filename = name + QString(".png");
	while ((size = m_size[sizeIndex]) != 0)
	{
		QString fullName = prefix + QString("%1x%1").arg(size) + QString("/") + filename;
		if (QFile::exists(fullName))
		{
			addFile(fullName, QSize(size, size));
			filesFound++;
		}
		sizeIndex++;
	}

	if (filesFound > 0 || !showUnknownIcon)
	{
		return;
	}

	sizeIndex = 0;
	filename = QString("unknown.png");
	while ((size = m_size[sizeIndex]) != 0)
	{
		QString fullName = prefix + QString("%1x%1").arg(size) + QString("/") + filename;
		if (QFile::exists(fullName))
		{
			addFile(fullName, QSize(size, size));
			filesFound++;
		}
		sizeIndex++;
	}
}

/*!
  Zrusenie ikony.
*/
SNIcon::~SNIcon()
{
}


