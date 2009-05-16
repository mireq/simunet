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
#include "SNDiagramDevice.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "SNDevicesDiagramScene.h"

#include "core/map/SNMapDeviceItem.h"
#include "core/map/SNPoint3f.h"

#include <QDebug>


SNDiagramConnector::SNDiagramConnector(SNDiagramDevice *device, qreal x, qreal y, port_num portNum, const QPen &pen, const QBrush &brush)
	: SNDiagramControlPoint(x, y, pen, brush)
{
	m_device = device;
	m_pos = QPointF(x, y);
	m_diff = QPointF(0, 0);
	m_hwPort = portNum;
	setFlag(ItemIsMovable, false);
	setPersistent(true);
}

SNDiagramConnector::~SNDiagramConnector()
{
}

void SNDiagramConnector::setPos(const QPointF &point)
{
	m_pos = point - m_diff;
	//SNDiagramControlPoint::setPos(point);
	m_device->setPos(m_pos);
}

void SNDiagramConnector::setDevicePos(const QPointF &point)
{
	m_pos = point;
	SNDiagramControlPoint::setPos(point + m_diff);
}

void SNDiagramConnector::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

void SNDiagramConnector::setItemDiff(const QPointF &diff)
{
	m_diff = diff;
	setDevicePos(m_pos);
	//setPos(m_pos);
}

SNDiagramDevice *SNDiagramConnector::device() const
{
	return m_device;
}

int SNDiagramConnector::type() const
{
	return Connector;
}

bool SNDiagramConnector::isFull() const
{
	if (m_leftSegment == NULL && m_rightSegment == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

port_num SNDiagramConnector::hwPowrt() const
{
	return m_hwPort;
}


/* ------------------------------------------------------------------ */

SNDiagramDevice::SNDiagramDevice(SNMapDeviceItem *device, SNDevicesDiagramScene *scene)
	:SNDiagramItem(), m_scene(scene)
{
	setFlag(ItemIsSelectable);
	m_device = device;
}


SNDiagramDevice::~SNDiagramDevice()
{
	/*SNDiagramConnector *conn;
	foreach(conn, m_connectors)
	{
		m_scene->removeControlPoint(conn, true);
	}*/
}

QRectF SNDiagramDevice::boundingRect() const
{
	return shape().controlPointRect();
}

void SNDiagramDevice::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->drawPath(shape());
	QFontMetrics fm(m_font);
	QRect textRect(0, 0, fm.width(m_name), fm.height());
	painter->drawText(textRect, Qt::AlignVCenter, m_name);
}

QPainterPath SNDiagramDevice::shape() const
{
	QPainterPath path;
	QFontMetrics fm(m_font);
	int width = fm.width(m_name);
	int height = fm.height();
	path.addRect(-1, -1, width + 2, height + 2);
	return path;
}

SNDiagramConnector *SNDiagramDevice::addConnector(port_num port)
{
	SNDiagramConnector *point = new SNDiagramConnector(this, 0, 0, port);
	m_connectors[port] = point;

	updateConnectorDiffs();
	point->setDevicePos(pos());

	return point;
}

void SNDiagramDevice::removeConnector(port_num port)
{
	QMap<port_num, SNDiagramConnector *>::iterator connector;
	connector = m_connectors.find(port);
	if (connector == m_connectors.end())
	{
		return;
	}
	else
	{
		SNDiagramConnector *oldConnector = connector.value();
		m_connectors.erase(connector);
		updateConnectorDiffs();

		/*m_scene->removeItem(oldConnector);
		delete oldConnector;*/
		m_scene->removeControlPoint(connector.value(), true);
	}
}

void SNDiagramDevice::setPos(const QPointF &pos)
{
	if (pos == this->pos())
	{
		return;
	}

	SNDiagramConnector *point;
	foreach(point, m_connectors)
	{
		point->setDevicePos(pos);
	}
	SNDiagramItem::setPos(pos);
	QPointF realPos = QGraphicsItem::pos();
	if (m_device != NULL)
	{
		m_device->setPos(SNPoint3f(realPos.x(), realPos.y(), m_device->pos().z()));
	}
}

void SNDiagramDevice::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

SNMapDeviceItem *SNDiagramDevice::device() const
{
	return m_device;
}

void SNDiagramDevice::setDevice(SNMapDeviceItem *device)
{
	m_device = device;
}

void SNDiagramDevice::updateConnectorDiffs()
{
	const QRectF bRect = boundingRect();
	const int width = bRect.width();
	const int height = bRect.height();
	const int pointSize = 10;

	QPointF diff;
	QMap<port_num, SNDiagramConnector *>::size_type numberPoints = m_connectors.size();
	SNDiagramConnector *p;

	diff.setX(width + pointSize + 5);
	diff.setY(height / 2 - (numberPoints - 1) * pointSize / 2);
	foreach(p, m_connectors)
	{
		p->setItemDiff(diff);
		diff.setY(diff.y() + pointSize);
	}
}

void SNDiagramDevice::setName(QString name)
{
	m_name = name;
	update();
	updateConnectorDiffs();
}

QString SNDiagramDevice::name() const
{
	return m_name;
}

const QMap< port_num, SNDiagramConnector *> *SNDiagramDevice::connectors() const
{
	return &m_connectors;
}

