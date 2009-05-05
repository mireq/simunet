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
#ifndef SNDIAGRAMDEVICE_H
#define SNDIAGRAMDEVICE_H

#include <QGraphicsItem>
#include <QList>

#include "SNDiagramLine.h"
#include "SNDiagramItem.h"

class SNDiagramDevice;
class SNMapDeviceItem;

/**
 @author Miroslav Bendik
 */
class SNDiagramConnector : public SNDiagramControlPoint
{
	public:
		SNDiagramConnector(SNDiagramDevice *device, qreal x, qreal y, const QPen &pen = QPen(), const QBrush &brush = QBrush());
		~SNDiagramConnector();

		void setPos(const QPointF &point);
		void setDevicePos(const QPointF &point);
		void setPos(qreal x, qreal y);
		void setItemDiff(const QPointF &diff);
		SNDiagramDevice *device() const;
		int type() const;
		bool isFull() const;

	private:
		SNDiagramDevice *m_device;
		QPointF m_diff;
		QPointF m_pos;
};


/**
 @author Miroslav Bendik
*/
class SNDiagramDevice : public SNDiagramItem
{
	public:
		SNDiagramDevice(SNMapDeviceItem *device);
		~SNDiagramDevice();
		QRectF boundingRect() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
		QPainterPath shape() const;
		SNDiagramConnector *addConnector();
		void setPos(const QPointF &pos);
		void setPos(qreal x, qreal y);
		void setItemDiff(QPointF diff);

		SNMapDeviceItem *device() const;
		void setDevice(SNMapDeviceItem *device);

	private:
		QList<SNDiagramConnector *> m_controlPoints;
		SNMapDeviceItem *m_device;

};

#endif
