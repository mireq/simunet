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

#include "gui/noqobject/diagram/SNDiagramLineSegment.h"
#include "gui/noqobject/diagram/SNDiagramDevice.h"
#include "core/map/SNMapLineItem.h"
#include "SNDevicesDiagramScene.h"

#include <QTimer>
#include <QDebug>


/*!
  \class SNDiagramLine
  \brief Kontajner obsahujuci zoznam kontrolnych bodov a segmentov ciary.
  \ingroup widgets
 */

/*!
  Vytvorenie novej ciary.

  \param[in] scene Scena, ktorej patri ciara.
  \param[in] mapLine Smernik na reprezentaciu ciary v mape.
 */
SNDiagramLine::SNDiagramLine(SNDevicesDiagramScene *scene, SNMapLineItem *mapLine)
	: QObject(), m_scene(scene), m_persistentPoints(0), m_mapLine(mapLine), m_fadeDuration(0), m_fadeValue(0), m_fadeInterval(100)
{
	m_hidePointsTimer = new QTimer(this);
	m_fadeTimer = new QTimer(this);
	connect(m_hidePointsTimer, SIGNAL(timeout()), SLOT(hideControlPoints()));
	connect(m_fadeTimer, SIGNAL(timeout()), SLOT(updateFade()));
}

/*!
  Zrusenie ciary (odstranenie segmentov ciary a kontrolnych bodov z sceny a
  ich dealokacia).
 */
SNDiagramLine::~SNDiagramLine()
{
	QList<SNDiagramControlPoint *> persPoints;
 
	SNDiagramControlPoint *point;
	foreach(point, m_controlPoints)
	{
		if (point->line() == this)
		{
			point->setLine(NULL);
			if (!point->persistent())
			{
				/*point->setLine(NULL);
				point->setLeftLineSegment(NULL);
				point->setRightLineSegment(NULL);*/
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

/*!
  Vrati styl ciary, ktorou su vykreslovane okraje kontrolnych bodov.
 */
QPen SNDiagramLine::controlPointsPen() const
{
	return m_controlPointsPen;
}

/*!
  Vrati styl pozadia kontrolnych bodov.
 */
QBrush SNDiagramLine::controlPointsBrush() const
{
	return m_controlPointsBrush;
}

/*!
  Vrati styl ciary prepajajucej kontrolne body.
 */
QPen SNDiagramLine::linePen() const
{
	return m_linePen;
}

/*!
  Nastavenie stylu ciary kontrolneho bodu.
 */
void SNDiagramLine::setControlPointsPen(const QPen &pen)
{
	m_controlPointsPen = pen;
	SNDiagramControlPoint *p;
	foreach (p, m_controlPoints)
	{
		p->setPen(pen);
	}
}

/*!
  Nastavenie stylu vyplne kontrolneho bodu.
 */
void SNDiagramLine::setControlPointsBrush(const QBrush &brush)
{
	m_controlPointsBrush = brush;
	SNDiagramControlPoint *p;
	foreach (p, m_controlPoints)
	{
		p->setBrush(brush);
	}
}

/*!
  Nastavenie stylu prepojovacej ciary.
 */
void SNDiagramLine::setLinePen(const QPen &pen)
{
	m_linePen = pen;
	SNDiagramLineSegment *l;
	foreach (l, m_lines)
	{
		l->setPen(pen);
	}
}

/*!
  Pridanie kontrolneho bodu do ciary.

  \param x Pozicia na X-ovej osi.
  \param y Pozicia na Y-ovej osi.
  \param pos Poradie, v ktorom sa vklada kontrolny bod do ciary.
  \param point Kontrolny bod, ktory sa ma pridat do ciary. Ak je hodnota \e NULL
    vytvori sa novy.
  \return Kontrolny bod, ktory bol pridany do ciary.
 */
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

/*!
  Pridanie kontrolneho bodu na poziciu [0, 0].

  \overload
 */
void SNDiagramLine::addControlPoint(SNDiagramControlPoint *point, int pos)
{
	addControlPoint(0, 0, pos, point);
}

/*!
  Rozdelenie segmentu ciary na 2 casti a vlozenie noveho kontrolneho bodu medzi ne.

  \param segment Segment, ktory sa ma "zlomit" na 2 casti.
  \param newPointPosition Pozicia noveho kontrolneho bodu.
 */
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

/*!
  Odstranenie kontrolneho bodu z prepojovacej ciary.

  \param point Kontrolny bod, ktory sa ma odstranit.
  \param pos Poradove cislo kontorlneho bodu, ktory odstranujeme.
  \param removePersistent Ak je \e true odstrani sa tento kontrolny bod bez ohladu
    na to, ci ma nastaveny atribut \e persistent.
 */
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

/*!
  Nastavenie viditelnosti kontrolnych bodov.
 */
void SNDiagramLine::setControlPointsVisible(bool visible)
{
	m_hidePointsTimer->stop();
	if (visible)
	{
		showControlPoints();
	}
	else
	{
		m_hidePointsTimer->start(1000);
	}
}

/*!
  \overload
 */
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

/*!
  Vrati pocet poloziek, ktore vlastni tato ciara (tj. nie perzistentnych).
 */
int SNDiagramLine::size() const
{
	return m_controlPoints.size() - m_persistentPoints;
}

/*!
  Vrati \e true v pripade, ze je tato ciara zbytocna (napr. obsahuje jediny
  kontrolny bod).
 */
bool SNDiagramLine::empty() const
{
	/*if (m_persistentPoints > 1)
	{
		return false;
	}
	if (m_controlPoints.size() - m_persistentPoints > 0)
	{
		return false;
	}*/
	if (m_controlPoints.size() >= 2)
	{
		return false;
	}
	return true;
}

/*!
  Vrati zoznam kontrolnych bodov.
 */
QVector<SNDiagramControlPoint *> SNDiagramLine::controlPoints() const
{
	return m_controlPoints;
}

/*!
  Vrati kontrolny bod, ktory je na pozicii urcenej argumentom \a pos.
 */
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

/*!
  Nastavenie polohy kontrolneho bodu.

  \param point Kontrolny bod, ktory presuvame.
  \param pos Cielova pozicia kontrolneho bodu.
 */
void SNDiagramLine::setPointPos(SNDiagramControlPoint *point, const QPointF &pos)
{
	point->setPos(pos);
}

/*!
  \overload

  \sa setPointPos
 */
void SNDiagramLine::movePoint(SNDiagramControlPoint *point, const QPointF &diff)
{
	setPointPos(point, point->pos() + diff);
}

/*!
  Skrytie kontrolnych bodov.
 */
void SNDiagramLine::hideControlPoints()
{
	m_hidePointsTimer->stop();
	m_fadeDuration = 5;
	m_fadeValue = 0;

	updateFade();
	m_fadeTimer->start(m_fadeInterval);
}

/*!
  Zobrazenie kontrolnych bodov.
 */
void SNDiagramLine::showControlPoints()
{
	m_fadeTimer->stop();

	SNDiagramControlPoint *point;
	foreach(point, m_controlPoints)
	{
		if (point->type() == SNDiagramControlPoint::ControlPoint)
		{
			point->setPen(m_controlPointsPen);
			point->setBrush(m_controlPointsBrush);
		}
	}

	setControlPointsVisiblePrivate(true);
}

/*!
  Aktualizacia fade efektu.
 */
void SNDiagramLine::updateFade()
{
	m_fadeValue++;
	if (m_fadeValue <= m_fadeDuration)
	{
		QColor lineColor = m_controlPointsPen.color();
		QColor brushColor = m_controlPointsBrush.color();
		lineColor.setAlpha(lineColor.alpha() * (m_fadeDuration - m_fadeValue) / m_fadeDuration);
		brushColor.setAlpha(brushColor.alpha() * (m_fadeDuration - m_fadeValue) / m_fadeDuration);
		QPen pen(lineColor);
		QBrush brush(brushColor);

		SNDiagramControlPoint *point;
		foreach(point, m_controlPoints)
		{
			if (point->type() == SNDiagramControlPoint::ControlPoint)
			{
				point->setPen(pen);
				point->setBrush(brush);
			}
		}
	}
	else
	{
		m_fadeTimer->stop();
		//setControlPointsVisiblePrivate(false);
		m_fadeValue = 0;
	}
}

/*!
  Nastavenie viditelnosti kontrolnych bodov.
 */
void SNDiagramLine::setControlPointsVisiblePrivate(bool visible)
{
	SNDiagramControlPoint *point;
	foreach(point, m_controlPoints)
	{
		if (point->type() == SNDiagramControlPoint::ControlPoint)
		{
			point->setVisible(visible);
		}
	}
}


