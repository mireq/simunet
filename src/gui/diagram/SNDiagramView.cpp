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
#include "SNDiagramView.h"

#include <QResizeEvent>
#include <QScrollBar>

#include <QDebug>

SNDiagramView::SNDiagramView(SNDevicesDiagramScene *scene, QWidget *parent): QGraphicsView(scene, parent)
{
}


SNDiagramView::SNDiagramView(QWidget* parent): QGraphicsView(parent)
{
}


SNDiagramView::~SNDiagramView()
{
}


void SNDiagramView::resizeEvent(QResizeEvent *event)
{
	/*QGraphicsScene *sc = scene();
	if (sc != NULL)
	{
		QRectF sceneRect = sc->sceneRect();
		sceneRect.setSize(event->size());
		QRectF bRect = sc->itemsBoundingRect();

		sc->setSceneRect(bRect.united(sceneRect));
	}*/

	QGraphicsScene *sc = scene();
	QSize sceneSize = viewport()->size();
	//sceneSize.setWidth(sceneSize.width() - horizontalScrollBar()->width());
	//sceneSize.setHeight(sceneSize.height() - verticalScrollBar()->height());
	if (horizontalScrollBar()->isVisible())
	{
		sceneSize.setHeight(sceneSize.height() + horizontalScrollBar()->height());
	}
	if (verticalScrollBar()->isVisible())
	{
		sceneSize.setWidth(sceneSize.width() + verticalScrollBar()->width());
	}

	static_cast<SNDevicesDiagramScene *>(sc)->resizeSceneView(sceneSize);
	QGraphicsView::resizeEvent(event);
}
