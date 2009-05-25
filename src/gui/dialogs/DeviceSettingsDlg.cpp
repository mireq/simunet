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
#include <QStatusTipEvent>
#include <QWebView>
#include <QProgressBar>

/*!
  \class DeviceSettingsDlg
  \brief Nastavenie vlastnosti zariadenia.
  \ingroup dialogs

  V tomto dialogu je mozne nastavovat zariadenia cez ich webove rozhranie,
  alebo cez telnet.
*/

/*!
  Vytvorenie dialogu pre dane zariadenie.

  \param devId Unikatne ID zariadenia pre ktore ma byt vytvoreny dialog.
  \param parent Nadradeny widget.
*/
DeviceSettingsDlg::DeviceSettingsDlg(int devId, QWidget* parent): QDialog(parent), m_deviceId(devId), m_errorsVisible(false), m_loadProgressVisible(false)
{
	m_statusBar = new QStatusBar;
	m_errorButton = new QPushButton(tr("Show javascript errors"));
	m_loadProgress = new QProgressBar;
	m_errorsTree = new QTreeWidget;
	m_webConfig = new SNWebConfigWidget(devId);
	SNTelnetWidget *tw = new SNTelnetWidget(devId);
	QTabWidget *tabs = new QTabWidget;
	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

	m_loadProgress->setFixedWidth(150);
	m_loadProgress->hide();
	m_statusBar->setFixedHeight(m_statusBar->sizeHint().height());
	m_statusBar->addPermanentWidget(m_loadProgress);
	m_statusBar->addPermanentWidget(m_errorButton);
	
	connect(m_webConfig, SIGNAL(jsErrorsAvitable(bool)), SLOT(setErrorsAvitable(bool)));
	connect(m_webConfig->page(), SIGNAL(linkHovered(QString, QString, QString)), SLOT(showWebLink(QString, QString, QString)));
	connect(m_webConfig->page(), SIGNAL(loadStarted()), SLOT(showProgressBar()));
	connect(m_webConfig->page(), SIGNAL(loadFinished(bool)), SLOT(hideProgressBar()));
	connect(m_webConfig->page(), SIGNAL(loadProgress(int)), SLOT(setProgress(int)));

	if (m_webConfig->errorsCount() == 0)
	{
		setErrorsAvitable(false);
	}
	else
	{
		setErrorsAvitable(true);
	}

	tabs->addTab(m_webConfig, tr("Web config"));
	tabs->addTab(tw, tr("Telnet"));

	int margin = layout->margin();
	QGridLayout *tabLayout = new QGridLayout();
	tabLayout->setMargin(margin);
	tabLayout->addWidget(tabs);
	layout->setMargin(0);
	layout->addLayout(tabLayout);
	layout->addWidget(m_statusBar, 0);

	m_errorsTree->hide();

	restoreWindowState();

	connect(tabs, SIGNAL(currentChanged(int)), SLOT(currentTabItemChanged(int)));
	connect(m_errorButton, SIGNAL(clicked()), SLOT(switchJsErrors()));

	m_webConfig->setUrl(QUrl("simunet:/"));
}

/*!
  Zrusenie konfiguracneho dialogu zariadenia.
*/
DeviceSettingsDlg::~DeviceSettingsDlg()
{
	saveWindowState();
}

/*!
  Vrati status bar okna.

  Status bar sluzi na zobrazovanie informacii ako ciel odkazu ktory je pod
  kurzorom, stav nacitavania, alebo pritomnost chyb v javascripte.
*/
QStatusBar *DeviceSettingsDlg::statusBar() const
{
	return m_statusBar;
}

/*!
  Vrati ID zariadenia ktoremu patri konfiguracny dialog.
*/
int DeviceSettingsDlg::deviceId() const
{
	return m_deviceId;
}

/*!
  \reimp
*/
bool DeviceSettingsDlg::event(QEvent *event)
{
	if (event->type() == QEvent::StatusTip)
	{
		m_statusBar->showMessage(static_cast<QStatusTipEvent *>(event)->tip());
		return true;
	}
	else if (event->type() == QEvent::Close)
	{
		emit dialogClosed(this);
	}
	return QDialog::event(event);
}

/*!
  Ulozenie vlastnosti okna pri zatvoreni.
*/
void DeviceSettingsDlg::saveWindowState()
{
	if (size() == m_initialSize)
	{
		return;
	}
	QSettings settings;
	settings.beginGroup("DeviceSettingsWindow");
	settings.setValue("size", size());
	settings.endGroup();
}

/*!
  Obnovenie ulozenych vlastnosti okna pri otvoreni.
*/
void DeviceSettingsDlg::restoreWindowState()
{
	QSettings settings;
	settings.beginGroup("DeviceSettingsWindow");
	m_initialSize = settings.value("size", QSize(500, 400)).toSize();
	resize(m_initialSize);
	settings.endGroup();
}

/*!
  Zmena zobrazeneho tabu.
*/
void DeviceSettingsDlg::currentTabItemChanged(int index)
{
	if (index == 0)
	{
		m_statusBar->show();
		m_webConfig->setUrl(m_webConfig->url());
	}
	else
	{
		m_statusBar->hide();
	}
}

/*!
  Zobrazenie, alebo skrytie chyb javascriptu ak uzivatel klikol na tlacidlo
  v statusbare zobrazovane pri existujucich chybach javascriptu.
*/
void DeviceSettingsDlg::switchJsErrors()
{
	if (m_errorsVisible)
	{
		hideJsErrors();
	}
	else
	{
		showJsErrors();
	}
}

/*!
  Zobrazenie chyb javascriptu.
*/
void DeviceSettingsDlg::showJsErrors()
{
	m_webConfig->setErrorsVisible(true);
	m_errorsVisible = true;
}

/*!
  Skrytie chyb javascriptu.
*/
void DeviceSettingsDlg::hideJsErrors()
{
	m_webConfig->setErrorsVisible(false);
	m_errorsVisible = false;
}

/*!
  Ak sa nastavi dostupnost chyb na \e true, potom sa v statusbare zobrazi
  tlacidlo na zobrazenie chyb.
*/
void DeviceSettingsDlg::setErrorsAvitable(bool avitable)
{
	m_errorButton->setVisible(avitable);
	if (!avitable)
	{
		hideJsErrors();
	}
}

/*!
  Zobrazenie odkazu v statusbare.
*/
void DeviceSettingsDlg::showWebLink(const QString &link, const QString &title, const QString &content)
{
	Q_UNUSED(content);
	if (!title.isNull())
	{
		m_statusBar->showMessage(title + QString(":") + link);
	}
	else
	{
		m_statusBar->showMessage(link);
	}
}

/*!
  Nastavenie hodnoty progressbaru v statusbare.
*/
void DeviceSettingsDlg::setProgress(int progress)
{
	if (!m_loadProgressVisible)
	{
		showProgressBar();
	}
	m_loadProgress->setValue(progress);
}

/*!
  Zobrazenie progressbaru (standardne pri zaciatku stahovania dat).
*/
void DeviceSettingsDlg::showProgressBar()
{
	m_loadProgress->show();
	m_loadProgressVisible = true;
}

/*!
  Skrytie progressbaru.
*/
void DeviceSettingsDlg::hideProgressBar()
{
	m_loadProgress->hide();
	m_loadProgressVisible = false;
}



