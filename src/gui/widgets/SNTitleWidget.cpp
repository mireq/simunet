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

SNTitleWidget::SNTitleWidget(QWidget *parent): QWidget(parent)
{
	setupGui();
}

SNTitleWidget::SNTitleWidget(const QString &text, QWidget *parent): QWidget(parent)
{
	setupGui();
	setText(text);
}

SNTitleWidget::SNTitleWidget(const QString &text, const QIcon &icon, QWidget *parent): QWidget(parent)
{
	setupGui();
	setText(text);
	setPixmap(icon);
}

SNTitleWidget::SNTitleWidget(const QString &text, const QString &iconName, QWidget *parent): QWidget(parent)
{
	setupGui();
	setText(text);
	setPixmap(iconName);
}

SNTitleWidget::SNTitleWidget(const QString &text, MessageType type, QWidget *parent): QWidget(parent)
{
	setupGui();
	setText(text);
	setPixmap(type);
}

SNTitleWidget::~SNTitleWidget()
{
	delete m_timer;
}

void SNTitleWidget::setPixmap(const QString &name)
{
	SNIcon icon(name);
	m_icon->setPixmap(icon.pixmap(32, 32));
	m_icon->show();
}

void SNTitleWidget::setPixmap(const QPixmap &pixmap)
{
	m_icon->setPixmap(pixmap);
	m_icon->show();
}

void SNTitleWidget::setPixmap(const QIcon &icon)
{
	m_icon->setPixmap(icon.pixmap(32, 32));
	m_icon->show();
}

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

void SNTitleWidget::setText(const QString &text)
{
	m_text->setText(text);
	m_text->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

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

