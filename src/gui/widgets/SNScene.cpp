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
#include "SNScene.h"

#include <math.h>

class SNSceneAttribDialog: public QDialog
{
	public:
		SNSceneAttribDialog(const QString &title);
		virtual ~SNSceneAttribDialog();

		void newPosition(const QPointF &ppisition);
		void newAngle(float angle);
	private:
		QLabel *posText;
		QLabel *angleText;
};

SNSceneAttribDialog::SNSceneAttribDialog(const QString &title)
{
	QLabel *posLabel = new QLabel("Position", this);
	QLabel *angleLabel = new QLabel("Angle", this);
	posText = new QLabel(this);
	angleText = new QLabel(this);

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(posLabel, 0, 0);
	layout->addWidget(posText, 0, 1);
	layout->addWidget(angleLabel, 1, 0);
	layout->addWidget(angleText, 1, 1);

	setLayout(layout);
	setWindowTitle(title);
}

SNSceneAttribDialog::~SNSceneAttribDialog()
{
}

void SNSceneAttribDialog::newPosition(const QPointF &position)
{
	QString text = QString::number(position.x(), 'f', 1) + QString(", ") + QString::number(position.y(), 'f', 1);
	posText->setText(text);
}

void SNSceneAttribDialog::newAngle(float angle)
{
	QString text = QString::number(angle, 'f', 1);
	angleText->setText(text);
}

//////////////////////////////////////////////

/*!
  \class SNScene
  \brief Graficka (OpenGL) scena.
*/


/*!
  Vytvorenie grafickej sceny.
*/
SNScene::SNScene(QObject* parent): QGraphicsScene(parent), m_tahanie(false), m_rotacia(0.0), m_pozicia(0.0, 0.0)
{
	m_dialog = new SNSceneAttribDialog("SNScene attributes");
	m_dialog->setWindowOpacity(0.8);
	addWidget(m_dialog);

	foreach (QGraphicsItem *item, items()) {
		item->setFlag(QGraphicsItem::ItemIsMovable);
		item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

		item->setPos(20, 30);
	}
	m_dialog->newAngle(m_rotacia);
	m_dialog->newPosition(m_pozicia);
}


/*!
  Zrusenie grafickej sceny.
*/
SNScene::~SNScene()
{
}

/*!
  Nastavenie navigacneho modu.
*/
void SNScene::setNavigationMode(NavigationMode mode)
{
	m_navigationMode = mode;
}


void SNScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (!m_tahanie)
	{
		QGraphicsScene::mouseMoveEvent(event);
	}

	// kontrola ci sme neklikli na dialog integrovany v opengl okne
	if (event->isAccepted())
	{
		return;
	}

	QFlags<Qt::MouseButton> buttons = event->buttons();
	if (buttons & (Qt::LeftButton | Qt::MidButton))
	{
		const QPointF delta = event->scenePos() - event->lastScenePos();
		NavigationMode mod = m_navigationMode;
		if (buttons & Qt::MidButton)
		{
			if (mod == Rotate)
			{
				mod = Move;
			}
			else
			{
				mod = Rotate;
			}
		}

		if (mod == Rotate)
		{
			zmenOtocenie(delta.x());
		}
		else
		{
			zmenPoziciu(0.1 * delta);
		}
		
		event->accept();
		update();
	}
}

void SNScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mousePressEvent(event);
	if (event->isAccepted())
	{
		return;
	}

	if (event->buttons() & (Qt::LeftButton | Qt::MidButton))
	{
		m_tahanie = true;
	}
}

void SNScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	m_tahanie = false;
}

void SNScene::keyPressEvent(QKeyEvent *event)
{
	QGraphicsScene::keyPressEvent(event);

	if (event->isAccepted())
	{
		return;
	}

	float krok = 0.4;
	int key = event->key();
	switch (key)
	{
		case Qt::Key_Left:
			event->accept();
			zmenPoziciu(QPointF(krok, 0.0));
			break;
		case Qt::Key_Right:
			zmenPoziciu(QPointF(-krok, 0.0));
			event->accept();
			break;
		case Qt::Key_Up:
			event->accept();
			zmenPoziciu(QPointF(0.0, krok));
			break;
		case Qt::Key_Down:
			event->accept();
			zmenPoziciu(QPointF(0.0, -krok));
			break;
	}
}

/*!
  Vykreslovanie pozadia sceny.

  Pri vykreslovani OpenGL ako pozadie nie je nutne pouzivat double buffering.
  K prekresleniu sceny dochadza az pri kompletnom vykresleni cez OpenGL.
*/
void SNScene::drawBackground(QPainter *painter, const QRectF &)
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
	glRotatef(m_rotacia, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0, 1.0f, 0.0f, 0.0f);
	glTranslatef(m_pozicia.x(), m_pozicia.y(), 0.0);

	vykreslenieMriezky();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glFlush();
}

void SNScene::zmenOtocenie(float rozdiel)
{
	m_rotacia += 90.0 * rozdiel / float(width());
	m_dialog->newAngle(m_rotacia);
}

void SNScene::zmenPoziciu(const QPointF &rozdiel)
{
	float uhol = m_rotacia * M_PI / 180.0;
	m_pozicia.setX(-rozdiel.x() * cos(uhol) - rozdiel.y() * sin(uhol) + m_pozicia.x());
	m_pozicia.setY(rozdiel.y() * cos(uhol) - rozdiel.x() * sin(uhol) + m_pozicia.y());
	m_dialog->newPosition(m_pozicia);
	update();
}

void SNScene::vykreslenieMriezky()
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
