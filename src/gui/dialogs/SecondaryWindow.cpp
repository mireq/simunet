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
#include "SecondaryWindow.h"

#include "SNDevicesListModel.h"
#include "DeviceSettingsDlg.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QTreeView>
#include <QAction>
#include <QMenu>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>


SecondaryWindow::SecondaryWindow(QWidget* parent, Qt::WindowFlags flags): QDockWidget(parent, flags), m_list(NULL), m_model(NULL)
{
	setObjectName("ToolWindow");
	setWindowTitle(tr("Tools"));

	m_tabWidget = new QTabWidget();
	m_tabWidget->setTabPosition(QTabWidget::West);
	setWidget(m_tabWidget);

	m_newAct = new QAction("New", this);
	m_newDirectoryAct = new QAction("New Directory", this);
	m_settingsAct = new QAction("Settings", this);
	m_deleteAct = new QAction("Delete", this);

	//QWidget *m_devicesWidget = new QWidget();
	//tabWidget->addTab(devicesWidget, tr("Devices"));
}


SecondaryWindow::~SecondaryWindow()
{
}

void SecondaryWindow::setModel(SNDevicesListModel *model)
{
	if (m_list == NULL)
	{
		m_model = model;
		m_list = new QTreeView();
		m_list->setIconSize(QSize(32, 32));
		m_list->setModel(model);
		m_list->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(m_list, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(showContextMenu(const QPoint &)));
		m_list->setSelectionMode(QAbstractItemView::SingleSelection);
		m_list->setDragEnabled(true);
		m_list->viewport()->setAcceptDrops(true);
		m_list->setDropIndicatorShown(true);
		m_list->setDragDropMode(QAbstractItemView::InternalMove);
		m_list->setSelectionModel(m_model->selectionModel());
		m_list->header()->hide();
		m_tabWidget->addTab(m_list, tr("Devices"));
	}
}

void SecondaryWindow::showContextMenu(const QPoint &point)
{
	if (m_list != NULL && m_model != NULL)
	{
		QList<QAction *> actions;
		QModelIndex index = m_list->indexAt(point);
		actions.append(m_newAct);
		actions.append(m_newDirectoryAct);
		if (index.isValid())
		{
			actions.append(m_settingsAct);
			actions.append(m_deleteAct);
		}
		if (actions.count() > 0)
		{
			QAction *triggered = QMenu::exec(actions, m_list->mapToGlobal(point));
			if (triggered == m_newAct)
			{
				m_model->startDevice("router", index);
			}
			else if (triggered == m_newDirectoryAct)
			{
				///@todo Poriadne anglicke texty
				bool ok;
				QString text = QInputDialog::getText(this,
				  tr("New folder name"),
				  tr("New folder name"), QLineEdit::Normal, "", &ok);
				
				if (ok && !text.isEmpty())
				{
					m_model->addDirectory(text, index);
				}
			}
			else if (triggered == m_settingsAct)
			{
				DeviceSettingsDlg *dlg = new DeviceSettingsDlg(this);
				dlg->exec();
				delete dlg;
			}
			else if (triggered == m_deleteAct)
			{
				if (m_model->rowCount(index) > 0)
				{
					///@todo Poriadny anglicky text
					QMessageBox::StandardButton btn = QMessageBox::question(this,
					  "Directory not empty", "Directory not empty, remove?",
					  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
					if (btn == QMessageBox::Yes)
					{
						m_model->removeDevice(index);
					}
				}
				else
				{
					m_model->removeDevice(index);
				}
			}
		}
	}
}


