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
#include "SNTelnetWidget.h"

#include "core/SNAccessors.h"
#include "core/SNSimulate.h"
#include "core/SNDynamicSettings.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QTextCharFormat>
#include <QPalette>
#include <QTextBlock>

#include <QDebug>

/*!
  \class SNTelnetWidget
  \brief Widget emulujuci terminal.
  \ingroup widgets

  Pomocou tohto widgetu sa konfiguruju zariadenia cez telnet.
*/

/*!
  Vytvorenie telnet widgetu pridelenému k zariadeniu \a devId.
*/
SNTelnetWidget::SNTelnetWidget(uint32_t devId, QWidget* parent):
		QWidget(parent), m_devId(devId)
{
	setObjectName("TelnetConfig");

	SNGuiSettings *settings = SNSingleton::getDynSettings<SNGuiSettings>();
	connect(settings, SIGNAL(termFontChanged(QFont)), SLOT(changeTermFont(QFont)));
	m_format.setFont(settings->guiFont(SNGuiSettings::TERM_FONT));
	/*m_font.setStyleHint(QFont::TypeWriter);
	m_format.setFont(m_font);*/

	m_document = new QPlainTextEdit();
	m_document->setCurrentCharFormat(m_format);
	m_document->textCursor().insertText("---\n");
	//m_document->installEventFilter(new SNTelnetEventFilter(this));
	m_document->installEventFilter(this);
	m_document->setTextInteractionFlags(Qt::TextEditable|Qt::TextSelectableByMouse);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_document);

	QPalette palette;
	palette.setColor(QPalette::Base, Qt::black);
	palette.setColor(QPalette::Text, Qt::white);
	m_document->setPalette(palette);
	connect(SNSingleton::getSimulate(),
	        SIGNAL(telnetResponseRecived(uint32_t, const char *, const char *)),
	        SLOT(processTelnetResponse(uint32_t, const char *, const char *)));
}

/*!
  Zrusenie telnet widgetu.
*/
SNTelnetWidget::~SNTelnetWidget()
{
}

/*!
  Vypis textu na obrazovku.

  Text sa moze vypisovat aj pocas zadavania textu uzivatelom. V tom pripade
  ale nebude zarucene, ze text ktory vidi uzivatel na obrazovke sa aj odosle.
  Taketo spravanie maju aj realne terminaly.
*/
void SNTelnetWidget::write(const QString &text)
{
	m_document->textCursor().clearSelection();
	m_document->moveCursor(QTextCursor::End);
	m_document->textCursor().insertText(text);
	m_document->ensureCursorVisible();
}

/*!
  \reimp
*/
void SNTelnetWidget::showEvent(QShowEvent *event)
{
	Q_UNUSED(event);
	m_document->setFocus();
}

/*!
  Vola sa pri odoslani riadku.
*/
void SNTelnetWidget::sendLineEvent(char controlChar)
{
	if (controlChar == '\n')
	{
		qDebug()<<m_currentLine;
	}
	else
	{
		qDebug()<<m_currentLine<<controlChar;
	}
	char *out = SNSingleton::getSimulate()->telnetRequest(m_devId, m_currentLine.toLatin1().data(), controlChar);
	PyObject *test = PyString_FromString("test");
	SNSingleton::getSimulate()->sendFrame(m_devId, test);
	write(out);
}


void SNTelnetWidget::processTelnetResponse(uint32_t id, const char *text, const char *cmd)
{
	if (id != m_devId)
	{
		return;
	}
	write(text);
	write(cmd);
}

void SNTelnetWidget::changeTermFont(const QFont &font)
{
	m_format.setFont(font);

	QTextCharFormat fmt;
	fmt.setFont(font);

	QTextCursor cursor = m_document->textCursor();
	cursor.select(QTextCursor::Document);
	cursor.mergeCharFormat(fmt);
	m_document->mergeCurrentCharFormat(fmt);
	m_document->textCursor().clearSelection();
	m_document->moveCursor(QTextCursor::End);
	m_document->setCurrentCharFormat(m_format);
}

const char *SNTelnetWidget::getControlChars() const
{
	return "?";
}

bool SNTelnetWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *ev = static_cast<QKeyEvent *>(event);
		int key = ev->key();

		m_document->textCursor().clearSelection();
		m_document->moveCursor(QTextCursor::End);
		m_document->ensureCursorVisible();
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
			int size = m_currentLine.size();
			if (size > 0)
			{
				m_currentLine.remove(size - 1, 1);
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
				sendLineEvent('\n');
				m_currentLine.clear();
				return true;
			}

			// prejdeme kontrolne znaky, ak najdeme zhodu odosleme text
			const char *controlChars = getControlChars();
			QString keyText = ev->text();
			char c;
			while ((c = *controlChars) != 0)
			{
				
				if (!keyText.isEmpty() && keyText.at(0).isPrint() && keyText == QString(c))
				{
					sendLineEvent(c);
					return true;
				}
				controlChars++;
			}

			// pridame znak do aktualne editovaneho riadku
			if (!keyText.isEmpty() && keyText.at(0).isPrint())
			{
				m_currentLine.append(keyText);
			}
		}
		return false;
	}
	return QWidget::eventFilter(obj, event);
}



