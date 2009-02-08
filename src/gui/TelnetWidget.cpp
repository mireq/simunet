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
	QVBoxLayout *layout = new QVBoxLayout(this);
	m_document = new QPlainTextEdit();
	m_font.setFamily("courier");
	m_font.setStyleHint(QFont::Courier);
	m_format.setFont(m_font);
	//m_document->setAcceptRichText(false);
	m_document->setCurrentCharFormat(m_format);
	m_document->textCursor().insertText("---\n");
	m_document->installEventFilter(new TelnetEventFilter(this));
	m_document->setTextInteractionFlags(Qt::TextEditable|Qt::TextSelectableByMouse);
	//m_document->setReadOnly(false);
	layout->addWidget(m_document);

	QPalette palette;
	palette.setColor(QPalette::Base, Qt::black);
	palette.setColor(QPalette::Text, Qt::white);
	m_document->setPalette(palette);
}


TelnetWidget::~TelnetWidget()
{
}

const char *TelnetWidget::getControlChars() const
{
	return "\n?";
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
			return false;
		}
		else if (key == Qt::Key_Backspace)
		{
			// nepovolime backspace na zaciatku raidku
			if (doc->textCursor().atBlockStart())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			const char *controlChars = w->getControlChars();
			QString keyText = ev->text();
			char c;
			while ((c = *controlChars) != 0)
			{
				if (c == '\n' && key == Qt::Key_Return)
				{
					qDebug()<<doc->textCursor().block().text();
				}
				else if (!keyText.isNull() && keyText == QString(c))
				{
					qDebug()<<doc->textCursor().block().text();
					return true;
				}
				controlChars++;
			}
		}
		return false;
	}
	return QObject::eventFilter(obj, event);
}

