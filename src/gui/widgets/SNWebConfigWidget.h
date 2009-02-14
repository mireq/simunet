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
#ifndef SNWEBCONFIGWIDGET_H
#define SNWEBCONFIGWIDGET_H

#include <QWidget>
#include <QString>
#include <QModelIndex>

class QWebView;
class QLabel;
class QSplitter;
class QTreeWidget;
class QWebPage;

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
		int errorsCount() const;
		void setErrorsVisible(bool visible);
		QWebPage *page();
	public slots:
		void setPageTitle(const QString &title);
		void setUrl(const QString &url);
		void addMenuItem(const QString &text, const QString &href, int depth, int mode = 0, const QString &title = QString());
	private:
		void loadJavascript();
		void saveWindowState();
		void restoreWindowState();
	private slots:
		void loadStarted();
		void loadFinished(bool ok);
		void addJavascriptObject();
		void addJsError(QString errorMsg, int line);
		void menuActivated(const QModelIndex &index);
	private:
		QSplitter *m_vSplitter;
		QSplitter *m_hSplitter;
		QWebView *m_view;
		QLabel *m_titleLabel;

		QTreeWidget *m_menuTree;
		QTreeWidget *m_jsErrorsTree;
		bool m_errorsVisible;

		static QString m_webConfigJs;
	signals:
		void jsErrorsAvitable(bool);
};

#endif
