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

#include <QNetworkRequest>
#include <QDebug>

SNWebConfigPage::SNWebConfigPage(QWidget *parent)
		: QWebPage(parent)
{
	settings()->setUserStyleSheetUrl(QUrl("qrc:/web/webstyle.css"));
	//setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
}


SNWebConfigPage::~SNWebConfigPage()
{
}

void SNWebConfigPage::javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID)
{
	emit javaScriptError(message, lineNumber);
}

bool SNWebConfigPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{
	qDebug()<<request.url();
	//return true;
	return QWebPage::acceptNavigationRequest(frame, request, type);
}


