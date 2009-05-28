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
#ifndef SNDIAGRAMLINESEGMENT_H
#define SNDIAGRAMLINESEGMENT_H

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
class SNMapLineItem;

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
/*!
  Lavy segment ciary v kontrolnom bode.
*/
		SNDiagramLineSegment *m_leftSegment;

/*!
  Pravy segment ciary v kontorlnom bode.
*/
		SNDiagramLineSegment *m_rightSegment;

/*!
  Kontajner, v ktorom sa nachadza tento kontrolny bod.
*/
		SNDiagramLine *m_line;

/*!
  Informacia o tom, ci je tento kontrolny bod perzistentny.
*/
		bool m_persistent;


/*!
  Styl, ktorym sa vykresluje obvod kontrolneho bodu.
*/
		QPen m_pen;

/*!
  Styl, ktorym sa vykresluje vnutorna cast kontrolneho bodu.
*/
		QBrush m_brush;

/*!
  Oblast, na ktorej sa vykresluje konektor.
*/
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


#endif
