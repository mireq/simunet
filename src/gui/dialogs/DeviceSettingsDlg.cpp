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
#include "DeviceSettingsDlg.h"

#include "SNTelnetWidget.h"
#include "SNWebConfigWidget.h"
#include <QBoxLayout>

#include <QTabWidget>
#include <QStatusBar>
#include <QPushButton>
#include <QTreeWidget>
#include <QSplitter>
#include <QSettings>

#include <QDebug>

DeviceSettingsDlg::DeviceSettingsDlg(QWidget* parent): QDialog(parent), m_errorsVisible(false)
{
	m_statusBar = new QStatusBar;
	m_errorButton = new QPushButton(tr("Show javascript errors"));
	m_errorsTree = new QTreeWidget;
	m_splitter = new QSplitter(this);
	SNWebConfigWidget *wc = new SNWebConfigWidget;
	SNTelnetWidget *tw = new SNTelnetWidget;
	QTabWidget *tabs = new QTabWidget;
	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

	m_statusBar->addPermanentWidget(m_errorButton);

	m_splitter->setOrientation(Qt::Vertical);
	tabs->addTab(wc, tr("Web config"));
	tabs->addTab(tw, tr("Telnet"));

	m_splitter->addWidget(tabs);
	m_splitter->addWidget(m_errorsTree);
	m_splitter->setCollapsible(0, false);
	layout->setMargin(0);
	layout->addWidget(m_splitter);
	layout->addWidget(m_statusBar, 0);

	m_errorsTree->hide();

	restoreWindowState();

	connect(tabs, SIGNAL(currentChanged(int)), SLOT(currentTabItemChanged(int)));
	connect(m_errorButton, SIGNAL(clicked()), SLOT(showJsErrors()));
}


DeviceSettingsDlg::~DeviceSettingsDlg()
{
	saveWindowState();
}

QStatusBar *DeviceSettingsDlg::statusBar()
{
	return m_statusBar;
}

void DeviceSettingsDlg::saveWindowState()
{
	QSettings settings;
	settings.beginGroup("DeviceSettingsWindow");
	if (m_errorsVisible)
	{
		settings.setValue("hsplitterstate", m_splitter->saveState());
	}
	settings.setValue("size", size());
	settings.endGroup();
}

void DeviceSettingsDlg::restoreWindowState()
{
	QSettings settings;
	settings.beginGroup("DeviceSettingsWindow");
	resize(settings.value("size", QSize(500, 400)).toSize());
	m_splitter->restoreState(settings.value("hsplitterstate").toByteArray());
	settings.endGroup();
}

void DeviceSettingsDlg::currentTabItemChanged(int index)
{
	if (index == 0)
	{
		m_statusBar->show();
		m_errorsTree->show();
	}
	else
	{
		m_statusBar->hide();
		m_errorsTree->hide();
	}
}

void DeviceSettingsDlg::showJsErrors()
{
	if (m_errorsVisible)
	{
		QSettings settings;
		settings.beginGroup("DeviceSettingsWindow");
		settings.setValue("hsplitterstate", m_splitter->saveState());
		settings.endGroup();
		m_errorsTree->hide();
		m_errorsVisible = false;
	}
	else
	{
		m_errorsTree->show();
		m_errorsVisible = true;
	}
}


