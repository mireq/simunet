/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2008 by Miroslav Bendik                                 *
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
#include "SNTitleWidget.h"
#include "SNIcon.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QColor>
#include <QTimer>

/*!
  \class SNTitleWidget
  \brief Widget na zobrazenie titulku / varovania v ramceku
  \ingroup widgets

  Tento widget je znamy hlavne z KDE 4. Pouziva sa na zobrazenie titulku
  roznych konfiguracnych dialogov alebo roznych hlaseni priamo v okne
  (ziadne modalne okno, tym je najlepsie sa vyhnut ak sa to da).
*/

/*!
  Vytvorenie prazdneho title widgetu.
*/
SNTitleWidget::SNTitleWidget(QWidget *parent): QWidget(parent)
{
	setupGui();
}

/*!
  Vytvorenie title widgetu s textom.
*/
SNTitleWidget::SNTitleWidget(const QString &text, QWidget *parent): QWidget(parent)
{
	setupGui();
	setText(text);
}

/*!
  Vytvorenie title widgetu s textom a ikonou.
*/
SNTitleWidget::SNTitleWidget(const QString &text, const QIcon &icon, QWidget *parent): QWidget(parent)
{
	setupGui();
	setText(text);
	setPixmap(icon);
}

/*!
  \overload
*/
SNTitleWidget::SNTitleWidget(const QString &text, const QString &iconName, QWidget *parent): QWidget(parent)
{
	setupGui();
	setText(text);
	setPixmap(iconName);
}

/*!
  \overload
*/
SNTitleWidget::SNTitleWidget(const QString &text, MessageType type, QWidget *parent): QWidget(parent)
{
	setupGui();
	setText(text);
	setPixmap(type);
}

/*!
  Zrusenie title widgetu.
*/
SNTitleWidget::~SNTitleWidget()
{
	delete m_timer;
}

/*!
  \overload
*/
void SNTitleWidget::setPixmap(const QString &name)
{
	SNIcon icon(name);
	m_icon->setPixmap(icon.pixmap(32, 32));
	m_icon->show();
}

/*!
  Nastavenie ikony zobrazenej vedla spravy.
*/
void SNTitleWidget::setPixmap(const QPixmap &pixmap)
{
	m_icon->setPixmap(pixmap);
	m_icon->show();
}

/*!
  \overload
*/
void SNTitleWidget::setPixmap(const QIcon &icon)
{
	m_icon->setPixmap(icon.pixmap(32, 32));
	m_icon->show();
}

/*!
  Zobrazenie preddefinovanej ikony.
*/
void SNTitleWidget::setPixmap(MessageType type)
{
	switch(type)
	{
		case InfoMessage: setPixmap("dialog-information"); break;
		case WarningMessage: setPixmap("dialog-warning"); break;
		case ErrorMessage: setPixmap("dialog-error"); break;
		case PlainMessage: break;
	}
}

/*!
  Nastavenie textu spravy.

  \param text Text spravy
*/
void SNTitleWidget::setText(const QString &text)
{
	m_text->setText(text);
	m_text->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}


/*!
  Nastavenie komentaru.

  \param comment Text komentaru.
  Tato funkcia nastavuje volitelny komentar. Pre zrusenie zobrazenia komentaru
  treba parameter comment nastavit na NULL (QString())
 */
void SNTitleWidget::setComment(const QString &comment)
{
	if (comment.isNull())
	{
		m_comment->hide();
	}
	else
	{
		m_comment->setText(comment);
		m_comment->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		m_comment->show();
	}
}


/*!
  Nastavenie casu po ktorom sprava zmizne.

  \param msecs Cas v milisekundach. Ak je cas nulovy bude sprava zobrazena permanentne.
 */
void SNTitleWidget::setHideTimeout(int msecs)
{
	if (msecs != 0)
	{
		m_timer->disconnect();
		m_timer->stop();
		connect(m_timer, SIGNAL(timeout()), SLOT(hide()));
		m_timer->start(msecs);
	}
	else
	{
		m_timer->disconnect();
		m_timer->stop();
	}
}

void SNTitleWidget::setupGui()
{
	m_timer = new QTimer();
	QVBoxLayout *layout = new QVBoxLayout(this);
	QFrame *frame = new QFrame();
	layout->addWidget(frame);

	QHBoxLayout *frameLayout = new QHBoxLayout();
	frame->setLayout(frameLayout);
	frame->setFrameShape(QFrame::StyledPanel);
	frame->setFrameShadow(QFrame::Sunken);
	frame->setAutoFillBackground(true);
	frame->setBackgroundRole(QPalette::Base);

	QVBoxLayout *textLayout = new QVBoxLayout();
	frameLayout->addLayout(textLayout, 1);

	m_text = new QLabel();
	m_comment = new QLabel();
	m_icon = new QLabel();

	QFont font = m_text->font();
	font.setBold(true);
	m_text->setFont(font);
	m_text->setWordWrap(true);
	m_comment->setWordWrap(true);


	textLayout->addWidget(m_text);
	textLayout->addWidget(m_comment);
	frameLayout->addWidget(m_icon);

	m_icon->hide();
	m_comment->hide();
}

