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
#include "MainWindow.h"
#include "Scene.h"
#include "AboutDlg.h"
#include "SecondaryWindow.h"

#include <QAction>
#include <QMenuBar>
#include <QApplication>
#include <QStatusBar>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QGLWidget>

class GraphicsView : public QGraphicsView
{
	protected:
		void resizeEvent(QResizeEvent *event) {
			if (scene())
				scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
			QGraphicsView::resizeEvent(event);
		}
};

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags)
{
	setObjectName("MainWindow");
	setWindowTitle(tr("SimuNet"));
	setupVariables();
	setupScene();
	setupUi();
	setupSecondaryWindow();
	restoreWindowState();
	statusBar()->showMessage(tr("Ready"), 5000);
}


MainWindow::~MainWindow()
{
	saveWindowState();
}

void MainWindow::setupVariables()
{
	m_aboutDlg = NULL;
}

void MainWindow::setupUi()
{
	setupActions();
	setupMenus();
	setupToolBars();
}

void MainWindow::setupToolBars()
{
	m_navigateToolBar = new QToolBar("Navigate Tool Bar");
	m_navigateToolBar->setObjectName("NavigateToolBar");

	m_navigateMoveAct = new QAction(tr("Move"), this);
	m_navigateRotateAct = new QAction(tr("Rotate"), this);
	m_navigateMoveAct->setIcon(QIcon(QPixmap(":move.png")));
	m_navigateRotateAct->setIcon(QIcon(QPixmap(":rotate.png")));

	m_navigateGroup = new QActionGroup(this);
	m_navigateGroup->setExclusive(true);
	m_navigateGroup->addAction(m_navigateMoveAct);
	m_navigateGroup->addAction(m_navigateRotateAct);

	connect(m_navigateGroup, SIGNAL(triggered(QAction *)), SLOT(sceneNavigationModeActionTriggered(QAction *)));

	m_navigateMoveAct->setCheckable(true);
	m_navigateRotateAct->setCheckable(true);
	m_navigateMoveAct->setChecked(true);
	sceneNavigationModeActionTriggered(m_navigateMoveAct);

	m_navigateToolBar->addAction(m_navigateMoveAct);
	m_navigateToolBar->addAction(m_navigateRotateAct);

	addToolBar(m_navigateToolBar);
}

void MainWindow::setupScene()
{
	m_scene = new Scene();
	GraphicsView *view = new GraphicsView;
	view->setFrameStyle(QFrame::NoFrame);
	view->setScene(m_scene);
	view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	view->show();
	setCentralWidget(view);
}

void MainWindow::setupSecondaryWindow()
{
	m_toolWindow = new SecondaryWindow(this);
	addDockWidget(Qt::RightDockWidgetArea, m_toolWindow);
}

void MainWindow::setupActions()
{
	m_quitAct = new QAction(tr("&Quit"), this);
	m_quitAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

	m_aboutAct = new QAction(tr("&About"), this);

	connect(m_quitAct, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(m_aboutAct, SIGNAL(triggered()), SLOT(about()));
}

void MainWindow::setupMenus()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));
	m_helpMenu = menuBar()->addMenu(tr("&Help"));

	m_fileMenu->addAction(m_quitAct);

	m_helpMenu->addAction(m_aboutAct);
}


void MainWindow::about()
{
	if (m_aboutDlg == NULL)
	{
		m_aboutDlg = new AboutDlg(this);
	}
	m_aboutDlg->show();
}

void MainWindow::sceneNavigationModeActionTriggered(QAction *action)
{
	if (action == m_navigateRotateAct)
	{
		m_scene->setNavigationMode(Scene::Rotate);
	}
	else
	{
		m_scene->setNavigationMode(Scene::Move);
	}
}

void MainWindow::restoreWindowState()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	restoreState(settings.value("state").toByteArray());
	resize(settings.value("size", QSize(600, 450)).toSize());
	settings.endGroup();
}

void MainWindow::saveWindowState()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("state", saveState());
	settings.setValue("size", size());
	settings.endGroup();
}