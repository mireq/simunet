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
#include "WebConfigWidget.h"
#include "WebConfigPage.h"

#include <QtGui>
#include <QtWebKit>
#include <QSettings>

#include <QDebug>

WebConfigWidget::WebConfigWidget(QWidget* parent): QWidget(parent)
{
	QGridLayout *layout = new QGridLayout;
	setLayout(layout);

	QFont font;
	font.setPointSizeF(font.pointSize()*1.5);
	font.setBold(true);

	m_titleLabel = new QLabel();
	layout->addWidget(m_titleLabel, 0, 0);
	m_titleLabel->setFont(font);
	m_titleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_titleLabel->setFrameShape(QFrame::StyledPanel);
	m_titleLabel->setTextFormat(Qt::PlainText);

	m_splitter = new QSplitter;
	m_splitter->setFrameShape(QFrame::StyledPanel);
	layout->addWidget(m_splitter, 1, 0);

	QTreeWidget *tree = new QTreeWidget;
	tree->setHeaderLabel(tr("Menu"));
	m_splitter->addWidget(tree);

	m_view = new QWebView;
	WebConfigPage *page = new WebConfigPage(this);
	m_view->setPage(page);
	connect(m_view->page(), SIGNAL(loadFinished(bool)), SLOT(loadFinished()));
	connect(m_view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavascriptObject()));

	QFile webConfigJsFile(":webconfig.js");
	if (!webConfigJsFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	m_webConfigJs = QString::fromUtf8(webConfigJsFile.readAll());
	
	QFile exampleFile(":examplesettings.html");
	if (!exampleFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	m_view->setHtml(QString::fromUtf8(exampleFile.readAll()));

	m_splitter->addWidget(m_view);
	restoreWindowState();
}


WebConfigWidget::~WebConfigWidget()
{
	saveWindowState();
}

void WebConfigWidget::render(const QString &string)
{
	m_view->setHtml(string);
}


void WebConfigWidget::setPageTitle(const QString &title)
{
	m_titleLabel->setText(title);
}

void WebConfigWidget::setUrl(const QString &url)
{
	qDebug()<<url;
}

void WebConfigWidget::saveWindowState()
{
	QSettings settings;
	settings.beginGroup("DeviceSettingsWindow");
	settings.setValue("vsplitterstate", m_splitter->saveState());
	settings.endGroup();
}

void WebConfigWidget::restoreWindowState()
{
	QSettings settings;
	settings.beginGroup("DeviceSettingsWindow");
	m_splitter->restoreState(settings.value("vsplitterstate").toByteArray());
	settings.endGroup();
}

void WebConfigWidget::loadFinished()
{
	QWebPage *page = m_view->page();
	QWebFrame *frame = page->mainFrame();
	frame->evaluateJavaScript(m_webConfigJs);
}

void WebConfigWidget::addJavascriptObject()
{
	m_view->page()->mainFrame()->addToJavaScriptWindowObject("webConfigWidget", this);
}



