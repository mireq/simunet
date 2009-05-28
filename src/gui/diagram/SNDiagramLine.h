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

#include <QObject>

#include <QBrush>
#include <QPen>

class SNDevicesDiagramScene;
class SNDiagramControlPoint;
class SNDiagramLineSegment;
class SNMapLineItem;

/**
  @author Miroslav Bendik
 */
class SNDiagramLine : public QObject
{
	Q_OBJECT
	public:
		SNDiagramLine(SNDevicesDiagramScene *scene, SNMapLineItem *mapLine);
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
		void removeControlPoint(SNDiagramControlPoint *controlPoint, bool removePersistent = false);
		int size() const;
		bool empty() const;
		QVector<SNDiagramControlPoint *> controlPoints() const;
		SNDiagramControlPoint *pointAt(int pos) const;
		void setPointPos(SNDiagramControlPoint *point, const QPointF &pos);
		void movePoint(SNDiagramControlPoint *point, const QPointF &diff);
		SNMapLineItem *mapLine() const;
		void setControlPointsVisible(bool visible);
	private slots:
		void hideControlPoints();
		void showControlPoints();
		void updateFade();

	private:
		void removeControlPoint(QVector<SNDiagramControlPoint *>::iterator point, int pos, bool removePersistent);
		void setControlPointsVisiblePrivate(bool visible);
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

		SNMapLineItem *m_mapLine;

		QTimer *m_hidePointsTimer;
		QTimer *m_fadeTimer;

		int m_fadeDuration;
		int m_fadeValue;
		int m_fadeInterval;

};

#endif
