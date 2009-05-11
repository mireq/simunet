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
#ifndef SNDIAGRAMLINE_H
#define SNDIAGRAMLINE_H

#include <QPen>
#include <QBrush>
#include <QVector>
#include <QColor>
#include <QLineF>
#include <QPointF>
#include <QGraphicsItemGroup>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>

#include "SNDiagramItem.h"

class SNDevicesDiagramScene;
class SNDiagramLineSegment;
class SNDiagramLine;

/**
 @author Miroslav Bendik
 */
class SNDiagramControlPoint : public SNDiagramItem
{
	public:

		/// Typ kontorlneho bodu.
		enum ControlPointType {
			ControlPoint, /*!< Standardny kontrolny bod na miestac spojov segmentov ciar. */
			Connector /*!< Konektor na zariadeni. */
		};
		SNDiagramControlPoint(qreal x, qreal y, const QPen &pen = QPen(), const QBrush &brush = QBrush());
		~SNDiagramControlPoint();

		virtual void setPos(const QPointF &point);
		virtual void setPos(qreal x, qreal y);
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
		virtual QRectF boundingRect() const;

		QPen pen() const;
		QBrush brush() const;
		void setPen(const QPen &pen);
		void setBrush(const QBrush &brush);

		SNDiagramLineSegment *leftLineSegment() const;
		SNDiagramLineSegment *rightLineSegment() const;
		void setLeftLineSegment(SNDiagramLineSegment *segment);
		void setRightLineSegment(SNDiagramLineSegment *segment);

		void setLine(SNDiagramLine *line);
		SNDiagramLine *line() const;

		bool persistent() const;
		void setPersistent(bool persistent);
		virtual bool isFull() const;
		virtual int type() const;

	protected:
		void updateLinePositions();

	protected:
		SNDiagramLineSegment *m_leftSegment;
		SNDiagramLineSegment *m_rightSegment;
		SNDiagramLine *m_line;
		bool m_persistent;

		QPen m_pen;
		QBrush m_brush;

		QRectF m_ellipseRect;
};

/**
 @author Miroslav Bendik
 */
class SNDiagramLineSegment : public QGraphicsLineItem
{
	public:
		SNDiagramLineSegment(SNDiagramLine *line, qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen = QPen());
		SNDiagramLineSegment(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen = QPen());
		QPainterPath shape() const;
		QRectF boundingRect() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
		SNDiagramLine *parentLine() const;
	private:
		SNDiagramLine *m_line;
};

/**
 @author Miroslav Bendik
*/
class SNDiagramLine
{
	public:
		SNDiagramLine(SNDevicesDiagramScene *scene);
		~SNDiagramLine();
		QPen controlPointsPen() const;
		QBrush controlPointsBrush() const;
		QPen linePen() const;
		void setControlPointsPen(const QPen &pen);
		void setControlPointsBrush(const QBrush &brush);
		void setLinePen(const QPen &pen);
		SNDiagramControlPoint *addControlPoint(qreal x, qreal y, int pos = -1, SNDiagramControlPoint *point = 0);
		void addControlPoint(SNDiagramControlPoint *point, int pos = -1);
		void breakLineSegment(SNDiagramLineSegment *segment, const QPointF &newPointPosition = QPointF());
//		void removeControlPoint(int pos);
		void removeControlPoint(QVector<SNDiagramControlPoint *>::iterator point, int pos, bool removePersistent);
		void removeControlPoint(SNDiagramControlPoint *controlPoint, bool removePersistent = false);
		int size() const;
		bool empty() const;
		QVector<SNDiagramControlPoint *> controlPoints() const;
		SNDiagramControlPoint *pointAt(int pos) const;
		void setPointPos(SNDiagramControlPoint *point, const QPointF &pos);
		void movePoint(SNDiagramControlPoint *point, const QPointF &diff);
	private:
		// okraj kontrolnych bodov
		QPen m_controlPointsPen;
		// vypln kontrolnyc bodov
		QBrush m_controlPointsBrush;
		// farba ciary medzi kontrolnymi bodmi
		QPen m_linePen;

		// zoznamy bodov
		QVector<SNDiagramControlPoint *> m_controlPoints;
		QVector<SNDiagramLineSegment *> m_lines;

		// scane a v ktorej je ciara
		SNDevicesDiagramScene *m_scene;

		// pocet bodov ktore priamo nepatria ciare (napr. konektory zariadeni)
		int m_persistentPoints;

};

#endif
