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
#include "SNConfigPanel.h"

#include <QMessageBox>

/*!
  \class SNConfigPanel
  \brief Konfiguracny panel.
  \ingroup widgets

  Od tejto triedy sa odvadzaju jednotlive konfiguracne panely.
*/

/*!
  Vytvorenie konfiguracneho panelu.
*/
SNConfigPanel::SNConfigPanel(QWidget* parent): QWidget(parent)
{
}

/*!
  Zrusenie konfiguracneho panelu.
*/
SNConfigPanel::~SNConfigPanel()
{
}

// ----------------------------------------------------------------

/*!
  \fn QString SNConfigPanel::panelName() const

  Nazov panelu zobrazujuci sa v zozname panelov.
 */

// ----------------------------------------------------------------

/*!
  \fn SNIcon SNConfigPanel::icon() const

  Ikona konfiguracneho panelu.
 */

// ----------------------------------------------------------------

/*!
  \fn void SNConfigPanel::saveChanges()

  Ulozenie zmien urobenych v konfiguracnom paneli.
 */

// ----------------------------------------------------------------

/*!
  Tato funkcia sa vola pri pokuse o prechod na iny panel.

  Pri zmene panelu je zvykom spytat sa uzivatela ci chce ulozit neulozene
  zmeny (ak take existuju). Ak sa ma prejst na iny panel vracia tato funkcia
  by mala vraciat true. Ak uzivatel klikol na to, ze nechce prejst na iny panl
  mala by vratit false.

  Priklad pouzitia:
\code
if (settingsChanged())
{
	QMessageBox::StandardButton btn = QMessageBox::question(
	this,
	tr("Unsaved Changes"),
	tr("The settings have been changed.\n"
	"Do you want to save your changes?"),
	QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard, QMessageBox::Save);
	switch (btn)
	{
		case QMessageBox::Cancel: return false; break;
		case QMessageBox::Discard: dropChanges(); break;
		case QMessageBox::Save: saveChanges(); break;
		default: break;
	}
}
return true;
\endcode
 */
bool SNConfigPanel::panelChanged()
{
	if (settingsChanged())
	{
		QMessageBox::StandardButton btn = QMessageBox::question(
			this,
			tr("Unsaved Changes"),
			tr("The settings have been changed.\n"
			"Do you want to save your changes?"),
		QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard, QMessageBox::Save);
		switch (btn)
		{
			case QMessageBox::Cancel: return false; break;
			case QMessageBox::Discard: dropChanges(); break;
			case QMessageBox::Save: saveChanges(); break;
			default: break;
		}
	}
	return true;
}

// ----------------------------------------------------------------

/*!
  \fn bool SNConfigPanel::panelSelected()

  Spusta sa pri prechode na tento panel.

  Ak nedojde k chybe pri prepinani panelu vracia tato funkcia true.
 */

// ----------------------------------------------------------------

/*!
  \fn void SNConfigPanel::info(QString msg, QString comment = QString(), int msecs = 0)

  Vyvolanie info spravy. Tato sprava sa zobrazi v dolnej casti konfigruacneho
  panelu.
 */

// ----------------------------------------------------------------

/*!
  \fn SNConfigPanel::warning(QString msg, QString comment = QString(), int msecs = 0)

  Vyvolanie varovnej spravy.

  \sa info()
 */

// ----------------------------------------------------------------

/*!
  \fn SNConfigPanel::error(QString msg, QString comment = QString(), int msecs = 0)

  Vyvolanie chybovej spravy.

  \sa info()
 */

// ----------------------------------------------------------------

/*!
  \fn SNConfigPanel::changed(bool)

  Singal sa vyvola pri zmene nastaveni.

  Pri zmene nastaveni ma hodnotu true. Ak uzivatel zmenil nastavenie na povodnu
  hodnotu vyvola false.
 */

// ----------------------------------------------------------------

/*!
  \fn SNConfigPanel::settingsChanged()

  Tato funkcia vracia \b true v pripade, ze boli urobene zmeny v danom
  konfiguracnom paneli. Ak ziadne zmeny urobene neboli vrati false.
 */

// ----------------------------------------------------------------

/*!
  \fn SNConfigPanel::dropChanges()

  Tato metoda sa vola v pripade, ze uzivatel nechce pouzit zmeny ktore
  urobil v konfigracnom paneli.
 */