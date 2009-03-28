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
#include "SNWebConfigPage.h"

#include <QWebSettings>
#include <QUrl>
#include <QWebFrame>

#include <QNetworkRequest>
#include <QDebug>

/*!
  \class SNWebConfigPage
  \brief Vylepsenie QWebPage o filtrovanie a pridany vlastny css styl.
  \ingroup widgets
*/

/*!
  Vytvorenie web stranky.
*/
SNWebConfigPage::SNWebConfigPage(QWidget *parent)
		: QWebPage(parent)
{
	settings()->setUserStyleSheetUrl(QUrl("qrc:/web/webstyle.css"));
	//setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
}

/*!
  Zrusenie web stranky.
*/
SNWebConfigPage::~SNWebConfigPage()
{
}

/*!
  \reimp

  \par Tato metoda emituje signal javaScriptError() v pripade, ze nastane chyba.
*/
void SNWebConfigPage::javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID)
{
	emit javaScriptError(message, lineNumber);
}

/*!
  \reimp

  \par Filter pre navigacne prikazy.
*/
bool SNWebConfigPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{
	qDebug()<<request.url();
	QUrl url = request.url();
	if (url.scheme() == "simunet")
	{
		/// TODO dopisat nastavenie html
		//mainFrame()->setHtml();
		return false;
	}
	return QWebPage::acceptNavigationRequest(frame, request, type);
}

/*!
  \fn SNWebConfigPage::javaScriptError(QString, int)
  Tento signal sa emituje pri najdeni chyby v javascripte.

  Emituje sa chybova hlaska a cislo riadku na ktorom doslo k chybe.
*/
