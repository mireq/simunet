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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QActionGroup;

class AboutDlg;
class ConfigureDlg;
class SNDevicesListModel;
class SNScene;
class SecondaryWindow;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
 @brief Hlavne okno aplikacie.
*/
class MainWindow : public QMainWindow
{
		Q_OBJECT
	public:
		MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
		~MainWindow();

	protected:
		void setupVariables();
		void setupUi();
		void setupToolBars();
		void setupSNScene();
		void setupSecondaryWindow();
		void setupActions();
		void setupMenus();
		void restoreWindowState();
		void saveWindowState();

	protected slots:
		void about();
		void configure();
		void sceneNavigationModeActionTriggered(QAction *);

	private:
		SNDevicesListModel *m_devicesModel;
		SNScene *m_scene;
		//menu
		QMenu *m_fileMenu;
		QMenu *m_settingsMenu;
		QMenu *m_helpMenu;

		// akcie
		QAction *m_quitAct;
		QAction *m_configureAct;
		QAction *m_aboutAct;
		// navigateToolBar
		QToolBar *m_navigateToolBar;
		QActionGroup *m_navigateGroup;
		QAction *m_navigateRotateAct;
		QAction *m_navigateMoveAct;

		// okna
		AboutDlg *m_aboutDlg;
		ConfigureDlg *m_configureDlg;
		SecondaryWindow *m_toolWindow;
};

#endif
