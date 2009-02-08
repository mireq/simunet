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
#include "TelnetWidget.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QTextCharFormat>
#include <QPalette>
#include <QTextBlock>

#include <QDebug>


TelnetWidget::TelnetWidget(QWidget* parent): QWidget(parent)
{
	m_font.setFamily("courier");
	m_font.setStyleHint(QFont::Courier);
	m_format.setFont(m_font);

	m_document = new QPlainTextEdit();
	m_document->setCurrentCharFormat(m_format);
	m_document->textCursor().insertText("---\n");
	m_document->installEventFilter(new TelnetEventFilter(this));
	m_document->setTextInteractionFlags(Qt::TextEditable|Qt::TextSelectableByMouse);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_document);

	QPalette palette;
	palette.setColor(QPalette::Base, Qt::black);
	palette.setColor(QPalette::Text, Qt::white);
	m_document->setPalette(palette);
}


TelnetWidget::~TelnetWidget()
{
}

void TelnetWidget::write(const QString &text)
{
	m_document->textCursor().insertText(text);
}

void TelnetWidget::showEvent(QShowEvent *event)
{
	m_document->setFocus();
}

void TelnetWidget::sendLineEvent(char controlChar)
{
	if (controlChar == '\n')
	{
		qDebug()<<m_currentLine;
	}
	else
	{
		qDebug()<<m_currentLine<<controlChar;
	}
}

const char *TelnetWidget::getControlChars() const
{
	return "?";
}

TelnetEventFilter::TelnetEventFilter(TelnetWidget *obj) : QObject(obj)
{
}

TelnetEventFilter::~ TelnetEventFilter()
{
}

bool TelnetEventFilter::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		TelnetWidget *w = static_cast<TelnetWidget *>(parent());

		QKeyEvent *ev = static_cast<QKeyEvent *>(event);
		int key = ev->key();

		QPlainTextEdit *doc = w->m_document;
		doc->textCursor().clearSelection();
		if (!doc->textCursor().atEnd())
		{
			doc->moveCursor(QTextCursor::End);
			doc->ensureCursorVisible();
		}
		if (key == Qt::Key_Up || key == Qt::Key_Down)
		{
			return true;
		}
		else if (key == Qt::Key_Left || key == Qt::Key_Right)
		{
			return true;
		}
		else if (key == Qt::Key_Backspace)
		{
			// nepovolime backspace na zaciatku raidku
			int size = w->m_currentLine.size();
			if (size > 0)
			{
				w->m_currentLine.remove(size - 1, 1);
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			if (key == Qt::Key_Return || key == Qt::Key_Enter)
			{
				w->sendLineEvent('\n');
				w->m_currentLine.clear();
				return false;
			}

			// prejdeme kontrolne znaky, ak najdeme zhodu odosleme text
			const char *controlChars = w->getControlChars();
			QString keyText = ev->text();
			char c;
			while ((c = *controlChars) != 0)
			{
				
				if (!keyText.isEmpty() && keyText.at(0).isPrint() && keyText == QString(c))
				{
					w->sendLineEvent(c);
					return true;
				}
				controlChars++;
			}

			// pridame znak do aktualne editovaneho riadku
			if (!keyText.isEmpty() && keyText.at(0).isPrint())
			{
				w->m_currentLine.append(keyText);
			}
		}
		return false;
	}
	return QObject::eventFilter(obj, event);
}

