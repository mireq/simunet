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
#include "core/map/SNMap.h"

#include "diagram/SNDevicesDiagramScene.h"
#include "diagram/SNDiagramView.h"

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
MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags),
	m_scene(NULL), m_2DView(NULL), m_3DView(NULL)
{
	m_settings = SNSingleton::getDynSettings<SNGuiSettings>();
	connect(m_settings, SIGNAL(antialiasingChanged(bool)), SLOT(setAntialiasing(bool)));
	qApp->setFont(m_settings->guiFont(SNGuiSettings::APP_FONT), "QWidget");

	m_splash = new SNSplash(":/splash_styles/default/");
	m_splash->show();
	m_splash->setDisplayed(true);
	m_splash->moveToCenter();
	qApp->processEvents();

	setObjectName("MainWindow");
	setWindowTitle(tr("SimuNet"));
	setupVariables();
	setupSNScene();
	setupUi();
	setupSecondaryWindow();
	restoreWindowState();

	setWindowsEnabled(false);
	QTimer::singleShot(0, this, SLOT(initSimuNet()));

	setAntialiasing(m_settings->antialiasing());
}

/*!
  Zrusenie hlavneho okna.
*/
MainWindow::~MainWindow()
{
	if (m_scene != NULL)
	{
		delete m_scene;
	}
	delete m_splash;
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

/*!
  Inicializacia premennych, ktore pouziva hlavne okno.
*/
void MainWindow::setupVariables()
{
	m_configureDlg = NULL;
	m_aboutDlg = NULL;
}

/*!
  Inicializacia GUI hlavneho okna.
*/
void MainWindow::setupUi()
{
	m_centralWidget = new QStackedWidget();
	setCentralWidget(m_centralWidget);

	setupActions();
	setupMenus();
	setupToolBars();
}

/*!
  Inicializacia poloziek tool baru.
*/
void MainWindow::setupToolBars()
{
	m_navigateToolBar = new QToolBar(tr("Navigate Tool Bar"));
	m_navigateToolBar->setObjectName("NavigateToolBar");

	m_2DAct = new QAction(tr("2D"), this);
	m_3DAct = new QAction(tr("3D"), this);

	m_navigateMoveAct = new QAction(tr("Move"), this);
	m_navigateRotateAct = new QAction(tr("Rotate"), this);
	m_navigateMoveAct->setIcon(SNIcon("move", false));
	m_navigateRotateAct->setIcon(SNIcon("rotate", false));

	m_graphicsViewGroup = new QActionGroup(this);
	m_graphicsViewGroup->setExclusive(true);
	m_graphicsViewGroup->addAction(m_2DAct);
	m_graphicsViewGroup->addAction(m_3DAct);

	m_navigateGroup = new QActionGroup(this);
	m_navigateGroup->setExclusive(true);
	m_navigateGroup->addAction(m_navigateMoveAct);
	m_navigateGroup->addAction(m_navigateRotateAct);

	connect(m_graphicsViewGroup, SIGNAL(triggered(QAction *)), SLOT(graphicsViewChanged(QAction *)));
	connect(m_navigateGroup, SIGNAL(triggered(QAction *)), SLOT(sceneNavigationModeChanged(QAction *)));

	m_navigateToolBar->addAction(m_2DAct);
	m_navigateToolBar->addAction(m_3DAct);

	m_navigateToolBar->addSeparator();

	m_navigateToolBar->addAction(m_navigateMoveAct);
	m_navigateToolBar->addAction(m_navigateRotateAct);

	addToolBar(m_navigateToolBar);

	m_2DAct->setCheckable(true);
	m_3DAct->setCheckable(true);
	m_2DAct->setChecked(true);
	graphicsViewChanged(m_2DAct);

	m_navigateMoveAct->setCheckable(true);
	m_navigateRotateAct->setCheckable(true);
	m_navigateMoveAct->setChecked(true);
	//sceneNavigationModeChanged(m_navigateMoveAct);
}

/*!
  Inicializacia grafickej sceny.
*/
void MainWindow::setupSNScene()
{
	m_diagram = new SNDevicesDiagramScene(this);
}

/*!
  Vytvorenie 3D sceny.
*/
void MainWindow::createSNScene()
{
	if (m_scene == NULL)
	{
		m_scene = new SNScene;
	}
}

/*!
  Inicializacia sekundarneho okna so zoznamom zariadeni.
*/
void MainWindow::setupSecondaryWindow()
{
	m_toolWindow = new SecondaryWindow(this);
	addDockWidget(Qt::RightDockWidgetArea, m_toolWindow);
}

/*!
  Inicializacia akcii (ukoncenie aplikacie, zobrazenie konfiguracie ...).
*/
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

/*!
  Inicializacia obsahu menu, pridanie akcii medzi polozky menu.
*/
void MainWindow::setupMenus()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));
	m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
	m_helpMenu = menuBar()->addMenu(tr("&Help"));

	m_fileMenu->addAction(m_quitAct);
	m_settingsMenu->addAction(m_configureAct);
	m_helpMenu->addAction(m_aboutAct);
}

/*!
  Zobrazenie informacii o oaplikacii.
*/
void MainWindow::about()
{
	if (m_aboutDlg == NULL)
	{
		m_aboutDlg = new AboutDlg(this);
	}
	m_aboutDlg->exec();
}

/*!
  Zobrazenie konfiguracie.
*/
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

/*!
  Zmena typu zobrazenia zariadeni (2D / 3D).
*/
void MainWindow::graphicsViewChanged(QAction *action)
{
	clearView();
	if (action == m_2DAct)
	{
		SNMap *map = SNSingleton::getMap();
		map->setScene(m_diagram);

		m_2DView = new SNDiagramView;
		m_2DView->setScene(m_diagram);
		m_2DView->setCacheMode(QGraphicsView::CacheBackground);
		//m_2DView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
		setAntialiasing(m_settings->antialiasing());
		m_centralWidget->addWidget(m_2DView);

		m_navigateToolBar->removeAction(m_navigateRotateAct);
		m_navigateToolBar->removeAction(m_navigateMoveAct);
	}
	else if (action == m_3DAct)
	{
		createSNScene();
		m_3DView = new GraphicsView;
		QGLWidget *gl = new QGLWidget(QGLFormat(QGL::SampleBuffers));
		if (!gl->isValid())
		{
			clearView();
			delete gl;
			QMessageBox::warning(this, tr("3D is not supported"), tr("3D mode is not supported by your graphic adapter or graphic driver."));
			m_2DAct->trigger();
			return;
		}
		m_3DView->setViewport(gl);
		m_3DView->setFrameStyle(QFrame::NoFrame);
		m_3DView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
		m_3DView->setScene(m_scene);
		m_centralWidget->addWidget(m_3DView);
		m_navigateToolBar->addAction(m_navigateRotateAct);
		m_navigateToolBar->addAction(m_navigateMoveAct);
	}
}

/*!
  Zmena rezimu navigacie v scene (lave tlacitko otacanie / presun v scene).
*/
void MainWindow::sceneNavigationModeChanged(QAction *action)
{
	createSNScene();
	if (action == m_navigateRotateAct)
	{
		m_scene->setNavigationMode(SNScene::Rotate);
	}
	else
	{
		m_scene->setNavigationMode(SNScene::Move);
	}
}

/*!
  Oneskorena inicializacia simulacneho jadra. Tato cast sa spusti az po
  zobrazeni GUI pre pocit vyssej odozvy aplikacie.
*/
void MainWindow::initSimuNet()
{
	m_splash->setProgress(50);
	m_splash->setMessage(tr("Starting devices"));
	qApp->processEvents();
	m_toolWindow->setModel(new SNDevicesListModel(this));
	/*for (int i = 0; i < 10; ++i)
	{
		m_devicesModel->startDevice("generic");
		qApp->processEvents();
	}*/
	setWindowsEnabled(true);
	statusBar()->showMessage(tr("Ready"), 5000);
	m_splash->setProgress(100);
	m_splash->setMessage(tr("Ready"));
	m_splash->closeDelayed();
}

/*!
  Nastavenie antialiasingu pre scenu.
*/
void MainWindow::setAntialiasing(bool antialiasing)
{
	if (m_2DView != NULL)
	{
		m_2DView->setRenderHint(QPainter::Antialiasing, antialiasing);
	}
}

/*!
  Obnovenie stavu hlavneho okna.
*/
void MainWindow::restoreWindowState()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	restoreState(settings.value("state").toByteArray());
	resize(settings.value("size", QSize(600, 450)).toSize());
	settings.endGroup();
}

/*!
  Ulozenie stavu hlavneho okna.
*/
void MainWindow::saveWindowState()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("state", saveState());
	settings.setValue("size", size());
	settings.endGroup();
}

/*!
  Vycistenie pohladu na zariadenia v pripade prepnutia medzi 2D / 3D rezimom.
*/
void MainWindow::clearView()
{
	if (m_2DView != NULL)
	{
		m_centralWidget->removeWidget(m_2DView);
		m_2DView->deleteLater();
		m_2DView = NULL;
	}
	if (m_3DView != NULL)
	{
		m_centralWidget->removeWidget(m_3DView);
		m_3DView->deleteLater();
		m_3DView = NULL;
	}
}


