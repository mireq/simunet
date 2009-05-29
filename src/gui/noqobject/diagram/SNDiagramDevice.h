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
#include <QMap>
#include <QFont>

#include "SNDiagramLineSegment.h"
#include "SNDiagramItem.h"
#include "sntypes.h"

class SNDiagramDevice;
class SNMapDeviceItem;

/**
 @author Miroslav Bendik
 */
class SNDiagramConnector : public SNDiagramControlPoint
{
	public:
		SNDiagramConnector(SNDiagramDevice *device, qreal x, qreal y, port_num portNum, const QPen &pen = QPen(), const QBrush &brush = QBrush());
		~SNDiagramConnector();

		void setPos(const QPointF &point);
		void setDevicePos(const QPointF &point);
		void setPos(qreal x, qreal y);
		void setItemDiff(const QPointF &diff);
		SNDiagramDevice *device() const;
		int type() const;
		bool isFull() const;
		port_num hwPowrt() const;

	private:
		SNDiagramDevice *m_device;
		QPointF m_diff;
		QPointF m_pos;
		port_num m_hwPort;
};


/**
 @author Miroslav Bendik
*/
class SNDiagramDevice : public SNDiagramItem
{
	public:
		SNDiagramDevice(SNMapDeviceItem *device, SNDevicesDiagramScene *scene);
		~SNDiagramDevice();
		QRectF boundingRect() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
		QPainterPath shape() const;
		SNDiagramConnector *addConnector(port_num port);
		void removeConnector(port_num port);
		void setPos(const QPointF &pos);
		void setPos(qreal x, qreal y);

		SNMapDeviceItem *device() const;
		void setDevice(SNMapDeviceItem *device);

		void setName(QString name);
		QString name() const;
		const QMap<port_num, SNDiagramConnector *> *connectors() const;

	private:
		void updateConnectorDiffs();
		void calcShape();

	private:
		SNMapDeviceItem *m_device;
		QMap<port_num, SNDiagramConnector *> m_connectors;
		QString m_name;
		QFont m_font;
		SNDevicesDiagramScene *m_scene;

/*!
  Oblast, ktora sa povazuje za oblast zariadenia.
 */
		QRect m_shapeRect;

/*!
  Pomocna premenna, pouziva sa pri zmenseni velkosti zariadenia. Vtedy sa
  najskor prekresli zariadenie pricom sa prekresluje oblast s vacsim obsahom
  nez udava shape.
 */
		QRect m_boundingRect;

};

#endif
