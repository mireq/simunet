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
#include "SNConfigPanel.h"
#include "ConfigureDlg.h"
#include "SNTitleWidget.h"

#include "SNIcon.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QPushButton>
#include <QCloseEvent>
#include <QShowEvent>

ConfigureDlg::ConfigureDlg(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	setWindowTitle(tr("Configure - SimuNet"));

	m_currentItem = NULL;
	m_previousItem = NULL;

	m_titleWidget = new SNTitleWidget();
	m_panelsList = new QListWidget();
	m_panelsList->setFixedWidth(200);
	m_panelsList->setIconSize(QSize(32, 32));
	m_panelsList->setUniformItemSizes(true);
	m_panelsList->setWordWrap(true);
	m_panelsList->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(m_panelsList, SIGNAL(itemSelectionChanged()), SLOT(listSelectionChanged()));

	m_panel = new QStackedWidget();

	QVBoxLayout *layout = new QVBoxLayout();
	QHBoxLayout *centerLayout = new QHBoxLayout();
	QVBoxLayout *panelLayout = new QVBoxLayout();

	layout->addLayout(centerLayout, 1);
	centerLayout->addWidget(m_panelsList);
	centerLayout->addLayout(panelLayout);
	panelLayout->addWidget(m_panel, 1);
	panelLayout->addWidget(m_titleWidget);

	m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Apply);
	layout->addWidget(m_buttons);
	setLayout(layout);

	m_titleWidget->hide();

	connect(m_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(onOkClicked()));
	connect(m_buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(onCancelClicked()));
	connect(m_buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), SLOT(onApplyClicked()));
}


ConfigureDlg::~ConfigureDlg()
{
}

void ConfigureDlg::addPanel(SNConfigPanel *panel)
{
	QListWidgetItem *newItem = new QListWidgetItem();
	newItem->setData(Qt::DisplayRole, panel->panelName());
	newItem->setData(Qt::DecorationRole, panel->icon());
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	m_panelsList->addItem(newItem);
	m_panel->addWidget(panel);
	if (m_panelsList->model()->rowCount(QModelIndex()) == 1)
	{
		m_panelsList->setCurrentRow(0);
	}
}

void ConfigureDlg::showInfo(const QString &text, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::InfoMessage);
	showMsg(text, QString(), msecs);
}

void ConfigureDlg::showInfo(const QString &text, const QString &comment, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::InfoMessage);
	showMsg(text, comment, msecs);
}

void ConfigureDlg::showWarning(const QString &text, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::WarningMessage);
	showMsg(text, QString(), msecs);
}

void ConfigureDlg::showWarning(const QString &text, const QString &comment, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::WarningMessage);
	showMsg(text, comment, msecs);
}

void ConfigureDlg::showError(const QString &text, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::ErrorMessage);
	showMsg(text, QString(), msecs);
}

void ConfigureDlg::showError(const QString &text, const QString &comment, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::ErrorMessage);
	showMsg(text, comment, msecs);
}

void ConfigureDlg::closeEvent(QCloseEvent *e)
{
	QWidget *w = m_panel->currentWidget();
	if (w == NULL)
	{
		return;
	}
	SNConfigPanel *panel = static_cast<SNConfigPanel *>(w);
	if (!panel->panelChanged())
	{
		e->ignore();
	}
}

void ConfigureDlg::showEvent(QShowEvent *e)
{
	m_currentItem = NULL;
	m_previousItem = NULL;
	m_titleWidget->setHideTimeout(0);
	m_titleWidget->hide();
	m_panelsList->setCurrentRow(0);
	listSelectionChanged();
}

void ConfigureDlg::showMsg(const QString &text, const QString &comment, int msecs)
{
	m_titleWidget->setText(text);
	m_titleWidget->setComment(comment);
	m_titleWidget->show();
	m_titleWidget->setHideTimeout(msecs);
}

void ConfigureDlg::connectPanel(SNConfigPanel *panel)
{
	connect(panel, SIGNAL(info(QString, QString, int)), SLOT(showInfo(QString, QString, int)));
	connect(panel, SIGNAL(warning(QString, QString, int)), SLOT(showWarning(QString, QString, int)));
	connect(panel, SIGNAL(error(QString, QString, int)), SLOT(showError(QString, QString, int)));
	connect(panel, SIGNAL(changed(bool)), m_buttons->button(QDialogButtonBox::Apply), SLOT(setEnabled(bool)));
}

void ConfigureDlg::disconnectPanel(SNConfigPanel *panel)
{
	panel->disconnect();
}

SNConfigPanel *ConfigureDlg::currentPanel()
{
	return static_cast<SNConfigPanel *>(m_panel->currentWidget());
}

void ConfigureDlg::listSelectionChanged()
{

	QListWidgetItem *pom = m_currentItem;
	m_currentItem = m_panelsList->currentItem();

	if (m_currentItem == NULL)
	{
		return;
	}

	bool changePanel = true;
	if (pom != NULL)
	{
		int prevRow = m_panelsList->row(pom);
		SNConfigPanel *panel = static_cast<SNConfigPanel *>(m_panel->widget(prevRow));
		changePanel = panel->panelChanged();
	}

	if (changePanel)
	{
		// odpojime signaly od predchadzajuceho panelu
		if (pom != NULL)
		{
			int prevRow = m_panelsList->row(pom);
			SNConfigPanel *prevPanel = static_cast<SNConfigPanel *>(m_panel->widget(prevRow));
			disconnectPanel(prevPanel);
		}

		int currRow = m_panelsList->row(m_currentItem);
		SNConfigPanel *panel = static_cast<SNConfigPanel *>(m_panel->widget(currRow));
		connectPanel(panel);
		panel->panelSelected();
		m_panel->setCurrentIndex(currRow);
		m_previousItem = m_currentItem;
	}
	else
	{
		m_panelsList->disconnect();
		if (pom != NULL)
		{
			m_panelsList->setCurrentItem(pom);
			m_previousItem->setSelected(true);
		}
		m_currentItem = m_previousItem;
		connect(m_panelsList, SIGNAL(itemSelectionChanged()), SLOT(listSelectionChanged()));
	}
}

void ConfigureDlg::onOkClicked()
{
	currentPanel()->saveChanges();
	accept();
}

void ConfigureDlg::onCancelClicked()
{
	if (currentPanel()->panelChanged())
	{
		reject();
	}
}

void ConfigureDlg::onApplyClicked()
{
	currentPanel()->saveChanges();
}


