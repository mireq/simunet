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
#ifndef SECONDARYWINDOW_H
#define SECONDARYWINDOW_H

#include <QDockWidget>
#include <QMap>

class DeviceSettingsDlg;
class SNDevicesListModel;
class QTreeView;
class QTabWidget;
class QAction;

/**
 @author Miroslav Bendik
*/
class SecondaryWindow : public QDockWidget
{
		Q_OBJECT
	public:
		SecondaryWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
		~SecondaryWindow();
		void setModel(SNDevicesListModel *model);

	private slots:
		void showContextMenu(const QPoint &point);
		void settingsClosed(DeviceSettingsDlg *dialog);
		void closeSettingsDialog(int devId);

	private:
		QAction *m_newAct;
		QAction *m_newDirectoryAct;
		QAction *m_settingsAct;
		QAction *m_deleteAct;
		QTreeView *m_list;
		SNDevicesListModel *m_model;
		QTabWidget *m_tabWidget;
		QMap<int, QWidget *> m_settingsDialogs;

};

#endif
