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
#ifndef SNWEBCONFIGPAGE_H
#define SNWEBCONFIGPAGE_H

#include <QWebPage>

/**
 @author Miroslav Bendik
*/
class SNWebConfigPage : public QWebPage
{
		Q_OBJECT
	public:
		SNWebConfigPage(QWidget *parent = 0);
		~SNWebConfigPage();
	protected:
		void javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID);
		bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);
	signals:
		void javaScriptError(const QString &error, int lineNum);
		void urlRequest(const QUrl &url);

};

#endif
