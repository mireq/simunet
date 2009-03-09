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
#ifndef SNTELNETWIDGET_H
#define SNTELNETWIDGET_H

#include <QWidget>
#include <QTextCharFormat>

class QPlainTextEdit;

/**
 @author Miroslav Bendik
*/
class SNTelnetWidget : public QWidget
{
		Q_OBJECT
	public:
		SNTelnetWidget(QWidget* parent = 0);
		~SNTelnetWidget();
	public slots:
		void write(const QString &text);
	protected:
		virtual void showEvent(QShowEvent *event);
		virtual void sendLineEvent(char controlChar);
	private:
		const char *getControlChars() const;
		bool eventFilter(QObject *obj, QEvent *event);
		QFont m_font;
		QTextCharFormat m_format;
		QPlainTextEdit *m_document;
		QString m_currentLine;
};


#endif
