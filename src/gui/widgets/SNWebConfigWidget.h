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
#ifndef WEBCONFIGWIDGET_H
#define WEBCONFIGWIDGET_H

#include <QWidget>
#include <QString>

class QWebView;
class QLabel;
class QSplitter;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/
class SNWebConfigWidget : public QWidget
{
		Q_OBJECT
	public:
		SNWebConfigWidget(QWidget* parent = 0);
		~SNWebConfigWidget();
		void render(const QString &string);
	public slots:
		void setPageTitle(const QString &title);
		void setUrl(const QString &url);
	private:
		void saveWindowState();
		void restoreWindowState();
	private slots:
		void loadFinished();
		void addJavascriptObject();
	private:
		QSplitter *m_splitter;
		QWebView *m_view;
		QLabel *m_titleLabel;
		QString m_webConfigJs;

};

#endif
