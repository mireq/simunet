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
#include "SNDiagramLine.h"
#include "SNDevicesDiagramScene.h"
#include "SNDiagramDevice.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPalette>

#include "core/map/SNMapLineItem.h"

#include <QDebug>

SNDiagramControlPoint::SNDiagramControlPoint(qreal x, qreal y, const QPen &pen, const QBrush &brush)
	: SNDiagramItem(), m_leftSegment(0), m_rightSegment(0), m_line(0), m_persistent(false)
{

	// -3.5, 8.0
	m_ellipseRect = QRectF(-3.5, -3.5, 8.0, 8.0);
	m_pen = pen;
	m_brush = brush;
	setFlag(ItemIsSelectable);
	setPos(QPointF(x, y));
}


SNDiagramControlPoint::~SNDiagramControlPoint()
{
}


void SNDiagramControlPoint::setPos(const QPointF &point)
{
	SNDiagramItem::setPos(point);
	updateLinePositions();
}

void SNDiagramControlPoint::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

void SNDiagramControlPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	if (isSelected())
	{
		painter->setPen(m_pen.color().darker());
		painter->setBrush(m_brush.color().darker());
	}
	else
	{
		painter->setPen(m_pen);
		painter->setBrush(m_brush);
	}
	painter->drawEllipse(m_ellipseRect);
}

QRectF SNDiagramControlPoint::boundingRect() const
{
	return m_ellipseRect;
}



QPen SNDiagramControlPoint::pen() const
{
	return m_pen;
}

QBrush SNDiagramControlPoint::brush() const
{
	return m_brush;
}

void SNDiagramControlPoint::setPen(const QPen &pen)
{
	m_pen = pen;
}

void SNDiagramControlPoint::setBrush(const QBrush &brush)
{
	m_brush = brush;
}

SNDiagramLineSegment *SNDiagramControlPoint::leftLineSegment() const
{
	return m_leftSegment;
}

SNDiagramLineSegment *SNDiagramControlPoint::rightLineSegment() const
{
	return m_rightSegment;
}

void SNDiagramControlPoint::setLeftLineSegment(SNDiagramLineSegment *segment)
{
	m_leftSegment = segment;
	updateLinePositions();
}

void SNDiagramControlPoint::setRightLineSegment(SNDiagramLineSegment *segment)
{
	m_rightSegment = segment;
	updateLinePositions();
}

void SNDiagramControlPoint::setLine(SNDiagramLine *line)
{
	m_line = line;
}

SNDiagramLine *SNDiagramControlPoint::line() const
{
	return m_line;
}

bool SNDiagramControlPoint::persistent() const
{
	return m_persistent;
}

void SNDiagramControlPoint::setPersistent(bool persistent)
{
	m_persistent = persistent;
}

bool SNDiagramControlPoint::isFull() const
{
	if (m_leftSegment == 0 || m_rightSegment == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int SNDiagramControlPoint::type() const
{
	return ControlPoint;
}

void SNDiagramControlPoint::updateLinePositions()
{
	if (m_leftSegment != NULL)
	{
		QLineF line = m_leftSegment->line();
		line.setP2(scenePos());
		m_leftSegment->setLine(line);
	}
	if (m_rightSegment != NULL)
	{
		QLineF line = m_rightSegment->line();
		line.setP1(scenePos());
		m_rightSegment->setLine(line);
	}
}

/* ------------------------------------------------------------------ */

SNDiagramLineSegment::SNDiagramLineSegment(SNDiagramLine *line, qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen)
	: QGraphicsLineItem(x1, y1, x2, y2)
{
	setPen(pen);
	setFlag(ItemIsSelectable);
	m_line = line;
}

SNDiagramLineSegment::SNDiagramLineSegment(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen)
	: QGraphicsLineItem(x1, y1, x2, y2)
{
	setPen(pen);
	setFlag(ItemIsSelectable);
	m_line = NULL;
}

QPainterPath SNDiagramLineSegment::shape() const
{
	QPainterPath path;
	path.moveTo(line().p1());
	path.lineTo(line().p2());

	QPainterPathStroker pStroker;
	pStroker.setWidth(pen().widthF() + 5.0);
	QPainterPath p = pStroker.createStroke(path);
	return p;
}

QRectF SNDiagramLineSegment::boundingRect() const
{
	return shape().controlPointRect();
}

void SNDiagramLineSegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	if (isSelected())
	{
		QPen p = pen();
		p.setColor(p.color().darker());
		p.setWidthF(p.widthF() + 2.0);
		painter->setPen(p);
	}
	else
	{
		painter->setPen(pen());
	}
	painter->drawLine(line());
}

SNDiagramLine *SNDiagramLineSegment::parentLine() const
{
	return m_line;
}

/* ------------------------------------------------------------------ */

SNDiagramLine::SNDiagramLine(SNDevicesDiagramScene *scene, SNMapLineItem *mapLine)
	: m_scene(scene), m_persistentPoints(0), m_mapLine(mapLine)
{
}


SNDiagramLine::~SNDiagramLine()
{
	QList<SNDiagramControlPoint *> persPoints;
 
	SNDiagramControlPoint *point;
	foreach(point, m_controlPoints)
	{
		if (point->line() == this)
		{
			point->setLine(NULL);
			point->setLeftLineSegment(NULL);
			point->setRightLineSegment(NULL);
			if (!point->persistent())
			{
				m_scene->removeItem(point);
				delete point;
			}
		}
		else if (point->persistent())
		{
			persPoints.append(point);
		}
	}
	SNDiagramLineSegment *seg;
	foreach (seg, m_lines)
	{
		foreach (point, persPoints)
		{
			if (point->leftLineSegment() == seg)
			{
				point->setLeftLineSegment(NULL);
			}
			if (point->rightLineSegment() == seg)
			{
				point->setRightLineSegment(NULL);
			}
		}
		m_scene->removeItem(seg);
		delete seg;
	}
}


QPen SNDiagramLine::controlPointsPen() const
{
	return m_controlPointsPen;
}

QBrush SNDiagramLine::controlPointsBrush() const
{
	return m_controlPointsBrush;
}

QPen SNDiagramLine::linePen() const
{
	return m_linePen;
}

void SNDiagramLine::setControlPointsPen(const QPen &pen)
{
	m_controlPointsPen = pen;
	SNDiagramControlPoint *p;
	foreach (p, m_controlPoints)
	{
		p->setPen(pen);
	}
}

void SNDiagramLine::setControlPointsBrush(const QBrush &brush)
{
	m_controlPointsBrush = brush;
	SNDiagramControlPoint *p;
	foreach (p, m_controlPoints)
	{
		p->setBrush(brush);
	}
}

void SNDiagramLine::setLinePen(const QPen &pen)
{
	m_linePen = pen;
	SNDiagramLineSegment *l;
	foreach (l, m_lines)
	{
		l->setPen(pen);
	}
}

SNDiagramControlPoint *SNDiagramLine::addControlPoint(qreal x, qreal y, int pos, SNDiagramControlPoint *point)
{
	if (pos < 0 || pos > m_controlPoints.size())
	{
		pos = m_controlPoints.size();
	}

	SNDiagramControlPoint *p;
	if (point == NULL)
	{
		p = new SNDiagramControlPoint(x, y, m_controlPointsPen, m_controlPointsBrush);
	}
	else
	{
		p = point;
	}
	p->setLine(this);

	SNDiagramConnector *conn = dynamic_cast<SNDiagramConnector *>(p);
	if (conn != NULL)
	{
		m_persistentPoints++;
		m_mapLine->addConnector(x, y, 0.0, conn->device()->device(), conn->hwPowrt(), pos);
	}
	else
	{
		m_mapLine->addControlPoint(x, y, 0.0, pos);
	}

	m_controlPoints.insert(pos, p);
	if (point == NULL)
	{
		m_scene->addItem(p);
	}

	if (m_controlPoints.size() > 1)
	{
		SNDiagramLineSegment *s = new SNDiagramLineSegment(this, 0, 0, 0, 0, m_linePen);

		if (pos + 1 == m_controlPoints.size())
		{
			m_lines.insert(pos - 1, s);
			m_controlPoints[pos - 1]->setRightLineSegment(s);
			m_controlPoints[pos]->setLeftLineSegment(s);
		}
		else if (pos == 0)
		{
			m_lines.insert(0, s);
			m_controlPoints[pos]->setRightLineSegment(s);
			m_controlPoints[pos + 1]->setLeftLineSegment(s);
		}
		else
		{
			m_lines.insert(pos - 1, s);
			m_controlPoints[pos - 1]->setRightLineSegment(s);
			m_controlPoints[pos]->setLeftLineSegment(s);
			m_controlPoints[pos]->setRightLineSegment(m_lines[pos]);
		}

		m_scene->addItem(s);

		s->setZValue(-1);
	}

	return p;
}

void SNDiagramLine::addControlPoint(SNDiagramControlPoint *point, int pos)
{
	addControlPoint(0, 0, pos, point);
}

void SNDiagramLine::breakLineSegment(SNDiagramLineSegment *segment, const QPointF &newPointPosition)
{
	int i = m_lines.indexOf(segment);
	if (i != -1)
	{
		addControlPoint(newPointPosition.x(), newPointPosition.y(), i + 1);
		m_scene->clearSelection();
		m_controlPoints[i + 1]->setSelected(true);
	}
}


void SNDiagramLine::removeControlPoint(QVector<SNDiagramControlPoint *>::iterator point, int pos, bool removePersistent)
{
	if (pos >= m_controlPoints.size())
	{
		return;
	}
	m_mapLine->removeControlPoint(pos);

	SNDiagramControlPoint *p = *point;
	m_controlPoints.erase(point);

	if (dynamic_cast<SNDiagramConnector *>(p) != NULL)
	{
		m_persistentPoints--;
	}

	if (p->type() != SNDiagramControlPoint::Connector || removePersistent)
	{
		m_scene->removeItem(p);
		delete p;
	}
	else
	{
		p->setLeftLineSegment(NULL);
		p->setRightLineSegment(NULL);
		p->setLine(NULL);
	}

	if (pos == 0)
	{
		if (m_lines.size() > 0)
		{
			SNDiagramLineSegment *l = *(m_lines.begin());
			m_scene->removeItem(l);
			m_lines.pop_front();
			delete l;

			(*m_controlPoints.begin())->setLeftLineSegment(NULL);
		}
		return;
	}

	SNDiagramLineSegment *l = m_lines[pos - 1];
	m_scene->removeItem(l);
	m_lines.remove(pos - 1);
	delete l;


	if (m_lines.size() <= pos - 1)
	{
		m_controlPoints[pos - 1]->setRightLineSegment(NULL);
		return;
	}

	l = m_lines[pos - 1];
	m_controlPoints[pos - 1]->setRightLineSegment(l);
	m_controlPoints[pos]->setLeftLineSegment(l);
	return;
}

void SNDiagramLine::removeControlPoint(SNDiagramControlPoint *controlPoint, bool removePersistent)
{
	QVector<SNDiagramControlPoint *>::size_type poradie = 0;
	QVector<SNDiagramControlPoint *>::iterator point;
	for (point = m_controlPoints.begin(); point != m_controlPoints.end(); ++point)
	{
		if (controlPoint == *point)
		{
			removeControlPoint(point, poradie, removePersistent);
			break;
		}
		poradie++;
	}
}

int SNDiagramLine::size() const
{
	return m_controlPoints.size() - m_persistentPoints;
}

bool SNDiagramLine::empty() const
{
 	if (m_persistentPoints > 1)
	{
		return false;
	}
	if (m_controlPoints.size() - m_persistentPoints > 0)
	{
		return false;
	}
	return true;
}

QVector<SNDiagramControlPoint *> SNDiagramLine::controlPoints() const
{
	return m_controlPoints;
}

SNDiagramControlPoint *SNDiagramLine::pointAt(int pos) const
{
	if (pos >= 0 && pos < m_controlPoints.size())
	{
		return m_controlPoints[pos];
	}
	else
	{
		return NULL;
	}
}

void SNDiagramLine::setPointPos(SNDiagramControlPoint *point, const QPointF &pos)
{
	point->setPos(pos);
}

void SNDiagramLine::movePoint(SNDiagramControlPoint *point, const QPointF &diff)
{
	setPointPos(point, point->pos() + diff);
}

SNMapLineItem *SNDiagramLine::mapLine() const
{
	return m_mapLine;
}
