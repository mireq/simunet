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
  \fn SNDynamicSettings::settingsChanged()

  Tento signal je emitovany pri zmene nastaveni v sbutriede SNDynamicSettings.
 */




