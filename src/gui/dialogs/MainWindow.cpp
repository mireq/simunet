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
#include "AboutDlg.h"
#include "ConfigureDlg.h"
#include "SNDevicesListModel.h"
#include "SNScene.h"
#include "SNSplash.h"
#include "SNIcon.h"
#include "SecondaryWindow.h"
#include "CfgApperance.h"
#include "CfgPerformance.h"

#include "core/SNAccessors.h"
#include "core/SNDynamicSettings.h"

#include <QAction>
#include <QMenuBar>
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

/*!
  \class MainWindow
  \brief Hlavne okno aplikacie.
  \ingroup dialogs
*/

/*!
  Vytvorenie hlavneho okna.
*/
MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags)
{
	SNGuiSettings *m_settings = SNSingleton::getDynSettings<SNGuiSettings>();
	qApp->setFont(m_settings->guiFont(SNGuiSettings::APP_FONT), "QWidget");

	m_splash = new SNSplash(":/splash_styles/default/");
	m_splash->show();
	m_splash->setDisplayed(true);
	m_splash->moveToCenter();

	setObjectName("MainWindow");
	setWindowTitle(tr("SimuNet"));
	setupVariables();
	setupSNScene();
	setupUi();
	setupSecondaryWindow();
	restoreWindowState();

	setWindowsEnabled(false);
	QTimer::singleShot(0, this, SLOT(initSimuNet()));

	SNSingleton::getDynSettings<SNGuiSettings>();
}

/*!
  Zrusenie hlavneho okna.
*/
MainWindow::~MainWindow()
{
	m_splash->deleteLater();
	saveWindowState();
	SNSingleton::deleteObjects();
}

/*!
  Nastavenie enabled / disabled stavu vsetkych zobrazenych okien.

  Tato metoda sa vyuziva pri operaciach kedy sa nesmu zariadenia modifikovat
  (napr. ukladanie). Vsetky otvorene okna sa vtedy prepnu do stavu disabled
  kedy sa v nich neda nic modifikovat.
*/
void MainWindow::setWindowsEnabled(bool enabled)
{
	foreach (QWidget *widget, QApplication::topLevelWidgets())
	{
		widget->setEnabled(enabled);
	}
}

void MainWindow::setupVariables()
{
	m_configureDlg = NULL;
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
	m_navigateMoveAct->setIcon(SNIcon("move", false));
	m_navigateRotateAct->setIcon(SNIcon("rotate", false));

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

void MainWindow::setupSNScene()
{
	m_scene = new SNScene();
	GraphicsView *view = new GraphicsView;
	view->setFrameStyle(QFrame::NoFrame);
	view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	view->setScene(m_scene);
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

	m_configureAct = new QAction(tr("&Configure SimuNet"), this);

	m_aboutAct = new QAction(tr("&About"), this);

	connect(m_quitAct, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(m_configureAct, SIGNAL(triggered()), SLOT(configure()));
	connect(m_aboutAct, SIGNAL(triggered()), SLOT(about()));
}

void MainWindow::setupMenus()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));
	m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
	m_helpMenu = menuBar()->addMenu(tr("&Help"));

	m_fileMenu->addAction(m_quitAct);
	m_settingsMenu->addAction(m_configureAct);
	m_helpMenu->addAction(m_aboutAct);
}


void MainWindow::about()
{
	if (m_aboutDlg == NULL)
	{
		m_aboutDlg = new AboutDlg(this);
	}
	m_aboutDlg->exec();
}

void MainWindow::configure()
{
	if (m_configureDlg == NULL)
	{
		m_configureDlg = new ConfigureDlg(this);
		m_configureDlg->addPanel(new CfgPerformance());
		m_configureDlg->addPanel(new CfgApperance());
	}
	m_configureDlg->exec();
}

void MainWindow::sceneNavigationModeActionTriggered(QAction *action)
{
	if (action == m_navigateRotateAct)
	{
		m_scene->setNavigationMode(SNScene::Rotate);
	}
	else
	{
		m_scene->setNavigationMode(SNScene::Move);
	}
}

void MainWindow::initSimuNet()
{
	m_splash->setProgress(50);
	m_splash->setMessage(tr("Starting devices"));
	m_devicesModel = new SNDevicesListModel(this);
	m_toolWindow->setModel(m_devicesModel);
	for (int i = 0; i < 10; ++i)
	{
		m_devicesModel->startDevice("router");
		qApp->processEvents();
	}
	setWindowsEnabled(true);
	statusBar()->showMessage(tr("Ready"), 5000);
	m_splash->setProgress(100);
	m_splash->setMessage(tr("Ready"));
	m_splash->closeDelayed();
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


