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
#include "SNWebConfigWidget.h"
#include "SNWebConfigPage.h"

#include <QtGui>
#include <QtWebKit>
#include <QSettings>

#include <QDebug>

QString SNWebConfigWidget::m_webConfigJs;

SNWebConfigWidget::SNWebConfigWidget(QWidget* parent): QWidget(parent), m_errorsVisible(false)
{
	setObjectName("WebConfig");
	QVBoxLayout *layout = new QVBoxLayout(this);

	QFont font;
	font.setPointSizeF(font.pointSize()*1.5);
	font.setBold(true);

	m_titleLabel = new QLabel;
	layout->addWidget(m_titleLabel);
	m_titleLabel->setFont(font);
	m_titleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_titleLabel->setFrameShape(QFrame::StyledPanel);
	m_titleLabel->setTextFormat(Qt::PlainText);
	layout->addWidget(m_titleLabel);

	m_vSplitter = new QSplitter;
	m_hSplitter = new QSplitter;
	m_hSplitter->setOrientation(Qt::Horizontal);
	m_vSplitter->setOrientation(Qt::Vertical);

	m_menuTree = new QTreeWidget(this);
	m_jsErrorsTree = new QTreeWidget(this);
	m_menuTree->setMouseTracking(true);
	m_menuTree->setObjectName("MenuTree");
	m_jsErrorsTree->setObjectName("JsErrorsTree");
	connect(m_menuTree, SIGNAL(doubleClicked(QModelIndex)), SLOT(menuActivated(QModelIndex)));

	
	SNWebConfigPage *page = new SNWebConfigPage;
	m_view = new QWebView;
	m_view->setPage(page);
	QFrame *webFrame = new QFrame(this);
	QGridLayout *webLayout = new QGridLayout(webFrame);
	webFrame->setFrameShape(QFrame::StyledPanel);
	webFrame->setFrameShadow(QFrame::Sunken);
	webLayout->setMargin(0);
	webLayout->addWidget(m_view);
	connect(m_view->page(), SIGNAL(loadFinished(bool)), SLOT(loadFinished(bool)));
	connect(m_view->page(), SIGNAL(loadStarted()), SLOT(loadStarted()));
	connect(m_view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavascriptObject()));
	connect(m_view->page(), SIGNAL(javaScriptError(QString, int)), SLOT(addJsError(QString, int)));
	//connect(m_view->page(), SIGNAL(linkHovered(QString,QString,QString)), SLOT(setStatusBarLink(QString)));

	loadJavascript();
	setUrl("url");

	m_hSplitter->addWidget(m_menuTree);
	m_hSplitter->addWidget(webFrame);
	m_vSplitter->addWidget(m_hSplitter);
	m_vSplitter->addWidget(m_jsErrorsTree);
	m_vSplitter->setCollapsible(0, false);

	m_menuTree->setHeaderLabel(tr("Menu"));
	QList<QString> jsHeaderItems;
	jsHeaderItems.append(QString(tr("Line")));
	jsHeaderItems.append(QString(tr("Error message")));
	m_jsErrorsTree->setHeaderLabels(jsHeaderItems);
	m_jsErrorsTree->setRootIsDecorated(false);
	QHeaderView *header = m_jsErrorsTree->header();
	header->setResizeMode(QHeaderView::Stretch);
	header->setResizeMode(0, QHeaderView::ResizeToContents);
	header->setAutoScroll(true);

	layout->addWidget(m_vSplitter);
	setLayout(layout);
	restoreWindowState();
}


SNWebConfigWidget::~SNWebConfigWidget()
{
	saveWindowState();
}

void SNWebConfigWidget::render(const QString &string)
{
	m_view->setHtml(string, QUrl("simunet:/"));
}

int SNWebConfigWidget::errorsCount() const
{
	return m_jsErrorsTree->topLevelItemCount();
}

void SNWebConfigWidget::setErrorsVisible(bool visible)
{
	if (!visible && m_jsErrorsTree->isVisible())
	{
		QSettings settings;
		settings.beginGroup("DeviceSettingsWindow");
		if (m_errorsVisible)
		{
			settings.setValue("vsplitterstate", m_vSplitter->saveState());
		}
		settings.endGroup();
	}
	m_errorsVisible = visible;
	m_jsErrorsTree->setVisible(visible);
}

QWebPage *SNWebConfigWidget::page()
{
	return m_view->page();
}

void SNWebConfigWidget::setPageTitle(const QString &title)
{
	m_titleLabel->setText(title);
}

void SNWebConfigWidget::setUrl(const QString &url)
{
	qDebug()<<url;
	QFile exampleFile(":web/examplesettings.html");
	if (!exampleFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	m_view->setHtml(QString::fromUtf8(exampleFile.readAll()), QUrl("simunet:/"));
	exampleFile.close();
}

void SNWebConfigWidget::addMenuItem(const QString &text, const QString &href, int depth, int mode, const QString &title)
{
	QFont newFont = font();
	newFont.setPointSizeF(double(8.0)/double(depth + 6.0) * newFont.pointSizeF());
	if (depth == 0)
	{
		newFont.setBold(true);
		QTreeWidgetItem *item = new QTreeWidgetItem;
		item->setData(0, Qt::DisplayRole, QVariant(text));
		item->setData(0, Qt::FontRole, QVariant(newFont));
		item->setData(0, Qt::UserRole, QVariant(href));
		item->setStatusTip(0, href);
		if (!title.isNull())
		{
			item->setData(0, Qt::ToolTipRole, QVariant(title));
		}
		m_menuTree->addTopLevelItem(item);
		if (mode == 2)
		{
			item->setSelected(true);
		}
		if (mode > 0)
		{
			item->setExpanded(true);
		}
		return;
	}
	QTreeWidgetItem *item = m_menuTree->topLevelItem(m_menuTree->topLevelItemCount()-1);
	if (!item)
	{
		return;
	}
	for (int i = 0; i < depth - 1; ++i)
	{
		int pocet = item->childCount();
		if (pocet == 0)
		{
			return;
		}

		if (mode > 0)
		{
			item->setExpanded(true);
		}
		item = item->child(pocet - 1);
		if (!item)
		{
			return;
		}
	}
	if (mode > 0)
	{
		item->setExpanded(true);
	}
	QTreeWidgetItem *child = new QTreeWidgetItem;
	child->setData(0, Qt::DisplayRole, QVariant(text));
	child->setData(0, Qt::FontRole, QVariant(newFont));
	child->setData(0, Qt::UserRole, QVariant(href));
	child->setStatusTip(0, href);
	if (!title.isNull())
	{
		child->setData(0, Qt::ToolTipRole, QVariant(title));
	}
	item->addChild(child);
	if (mode > 0)
	{
		child->setExpanded(true);
	}
	if (mode == 2)
	{
		child->setSelected(true);
	}
}

void SNWebConfigWidget::loadJavascript()
{
	if (!m_webConfigJs.isNull())
	{
		return;
	}

	QFile webConfigJsFile(":web/webconfig.js");
	if (!webConfigJsFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	m_webConfigJs = QString::fromUtf8(webConfigJsFile.readAll());
	webConfigJsFile.close();
}

void SNWebConfigWidget::saveWindowState()
{
	QSettings settings;
	settings.beginGroup("DeviceSettingsWindow");
	settings.setValue("hsplitterstate", m_hSplitter->saveState());
	if (m_errorsVisible)
	{
		settings.setValue("vsplitterstate", m_vSplitter->saveState());
	}
	settings.endGroup();
}

void SNWebConfigWidget::restoreWindowState()
{
	QSettings settings;
	settings.beginGroup("DeviceSettingsWindow");
	m_vSplitter->restoreState(settings.value("vsplitterstate").toByteArray());
	m_hSplitter->restoreState(settings.value("hsplitterstate").toByteArray());
	settings.endGroup();
}


void SNWebConfigWidget::loadStarted()
{
	emit jsErrorsAvitable(false);
	m_jsErrorsTree->clear();
}

void SNWebConfigWidget::loadFinished(bool ok)
{
	if (ok)
	{
		m_menuTree->clear();
		QWebPage *page = m_view->page();
		QWebFrame *frame = page->mainFrame();
		frame->evaluateJavaScript(m_webConfigJs);
	}
}

void SNWebConfigWidget::addJavascriptObject()
{
	m_view->page()->mainFrame()->addToJavaScriptWindowObject("webConfigWidget", this);
}

void SNWebConfigWidget::addJsError(QString errorMsg, int line)
{
	QTreeWidgetItem *errorLine = new QTreeWidgetItem;
	errorLine->setData(0, Qt::DisplayRole, QVariant(line));
	errorLine->setData(1, Qt::DisplayRole, QVariant(errorMsg));
	errorLine->setData(1, Qt::ToolTipRole, QVariant(errorMsg));
	if (m_jsErrorsTree->topLevelItemCount() == 0)
	{
		emit jsErrorsAvitable(true);
	}
	m_jsErrorsTree->addTopLevelItem(errorLine);
}

void SNWebConfigWidget::menuActivated(const QModelIndex &index)
{
	if (index.isValid())
	{
		setUrl(index.data(Qt::UserRole).toString());
	}
}


