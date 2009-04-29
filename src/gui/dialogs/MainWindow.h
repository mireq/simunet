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

class QActionGroup;
class QAction;
class QMenu;
class QStackedWidget;
class QGraphicsView;

class AboutDlg;
class ConfigureDlg;
class GraphicsView;
class SNDevicesListModel;
class SNScene;
class SNSplash;
class SecondaryWindow;
class SNDevicesDiagramScene;

/**
 @author Miroslav Bendik
*/
class MainWindow : public QMainWindow
{
		Q_OBJECT
	public:
		MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
		~MainWindow();
		static void setWindowsEnabled(bool enabled);

	private:
		void setupVariables();
		void setupUi();
		void setupToolBars();
		void setupSNScene();
		void setupSecondaryWindow();
		void setupActions();
		void setupMenus();
		void restoreWindowState();
		void saveWindowState();
		void clearView();

	private slots:
		void about();
		void configure();
		void graphicsViewChanged(QAction *);
		void sceneNavigationModeChanged(QAction *);
		void initSimuNet();

	private:
		SNDevicesListModel *m_devicesModel;
		SNScene *m_scene;
		SNDevicesDiagramScene *m_diagram;
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

		QActionGroup *m_graphicsViewGroup;
		QAction *m_2DAct;
		QAction *m_3DAct;

		// okna
		AboutDlg *m_aboutDlg;
		ConfigureDlg *m_configureDlg;
		SecondaryWindow *m_toolWindow;
		SNSplash *m_splash;

		// centralna cast
		QStackedWidget *m_centralWidget;
		QGraphicsView *m_2DView;
		GraphicsView *m_3DView;
};

#endif
