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
#include "Scene.h"

#include <QDebug>

class SceneDialog: public QDialog
{
	public:
		SceneDialog(const QString &title);
		virtual ~SceneDialog();
};

SceneDialog::SceneDialog(const QString &title)
{
	QVBoxLayout *layout = new QVBoxLayout;
	setLayout(layout);
	setWindowTitle(title);
}

SceneDialog::~SceneDialog()
{
}

Scene::Scene(QObject* parent): QGraphicsScene(parent), rotacia(0.0)
{
	SceneDialog *dialog = new SceneDialog("Dialog");
	dialog->setWindowOpacity(0.8);

	dialog->layout()->addWidget(new QLabel("Ukazkovy dialog v 3D scene"));

	addWidget(dialog);

	foreach (QGraphicsItem *item, items()) {
		item->setFlag(QGraphicsItem::ItemIsMovable);
		item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

		item->setPos(20, 30);
	}
}


Scene::~Scene()
{
}


void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseMoveEvent(event);

	// kontrola ci sme neklikli na dialog integrovany v opengl okne
	if (event->isAccepted())
	{
		return;
	}

	if (event->buttons() & Qt::LeftButton)
	{
		const QPointF delta = event->scenePos() - event->lastScenePos();
		rotacia += 90.0 * delta.x() / float(width());
		event->accept();
		update();
	}
}


void Scene::drawBackground(QPainter *painter, const QRectF &)
{
	if (painter->paintEngine()->type() != QPaintEngine::OpenGL) {
		qWarning("Potrebujeme ako viewport QGLWidget");
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	float pomer = width() / height();
	glFrustum(1.0 * pomer, -1.0 * pomer, 1.0, -1.0, 2.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//glTranslatef(0.0f, 0.0f, -500.0f);
	glRotatef(rotacia, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0, 1.0f, 0.0f, 0.0f);

	vykreslenieMriezky();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glFlush();
}

void Scene::vykreslenieMriezky()
{
	int vzdialenostZ = -10;
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	for (int i = -50; i < 51; i++)
	{
		glVertex3i(i * 10,-500, vzdialenostZ);
		glVertex3i(i * 10, 500, vzdialenostZ);
	}
	for (int i = -50; i < 51; i++)
	{
		glVertex3i(-500, i * 10, vzdialenostZ);
		glVertex3i( 500, i * 10, vzdialenostZ);
	}
	glEnd();
}
