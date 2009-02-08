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
#ifndef TELNETWIDGET_H
#define TELNETWIDGET_H

#include <QWidget>
#include <QTextCharFormat>

class QPlainTextEdit;
class TelnetEventFilter;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/
class TelnetWidget : public QWidget
{
		Q_OBJECT
	public:
		TelnetWidget(QWidget* parent = 0);
		~TelnetWidget();
	private:
		const char *getControlChars() const;
		QFont m_font;
		QTextCharFormat m_format;
		QPlainTextEdit *m_document;
		friend class TelnetEventFilter;
};

class TelnetEventFilter : public QObject
{
		Q_OBJECT
	public:
		TelnetEventFilter(TelnetWidget *obj = 0);
		~TelnetEventFilter();
	protected:
		bool eventFilter(QObject *obj, QEvent *event);
};

#endif
