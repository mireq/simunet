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

Scene::Scene(QWidget* parent): QGLWidget(parent)
{
	/// TODO: Implementovat
	setMinimumSize(100, 100);
}


Scene::~Scene()
{
}

void Scene::initializeGL()
{
	qglClearColor(Qt::black);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}

void Scene::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float pomer = (float)w / (float)h;

	glFrustum(1.0 * pomer, -1.0 * pomer, 0.5, -0.5, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void Scene::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	qglColor(Qt::red);
	glLoadIdentity();
	glTranslatef(-300.0f, 0.0f,-600.0f);
	glRotatef(85.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-11.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
		for (int i = 0; i < 51; i++)
		{
			glVertex3i(i * 10,  0,  0);
			glVertex3i(i * 10, 500, 0);
		}
		for (int i = 0; i < 51; i++)
		{
			glVertex3i(0,  i * 10,  0);
			glVertex3i(500, i * 10, 0);
		}
	glEnd();
	glFlush();
}

