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

/*!
  \class SNDiagramControlPoint
  \brief Kontrolny bod v grafickej scene.
  \ingroup widgets

  Tato trieda vykresluje kontrolne body v zlomoch ciary spajajucej zariadenia.
*/

/*!
  Vytvorenie noveho kontrolneho bodu na pozicii [\a x, \a y]. Styl ciary
  obvodu konektoru je urceny argumentom \a pen. Vypln je urcena argumentom
  \a brush.
*/
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

/*!
  Zrusenie kontrolneho bodu.
*/
SNDiagramControlPoint::~SNDiagramControlPoint()
{
}

/*!
  Nastavenie pozicie kontorlneho bodu na novu poziciu \a point.
*/
void SNDiagramControlPoint::setPos(const QPointF &point)
{
	SNDiagramItem::setPos(point);
	updateLinePositions();
}

/*!
  \overload
*/
void SNDiagramControlPoint::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

/*!
  \reimp
*/
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

/*!
  \reimp
*/
QRectF SNDiagramControlPoint::boundingRect() const
{
	return m_ellipseRect;
}

/*!
  Vrati styl obvodu konektoru.
*/
QPen SNDiagramControlPoint::pen() const
{
	return m_pen;
}

/*!
  Vrati styl vyplne konektoru.
*/
QBrush SNDiagramControlPoint::brush() const
{
	return m_brush;
}

/*!
  Nastavenie stylu obvodu konektoru.
*/
void SNDiagramControlPoint::setPen(const QPen &pen)
{
	m_pen = pen;
}

/*!
  Nastavenie stylu vyplne konektoru.
*/
void SNDiagramControlPoint::setBrush(const QBrush &brush)
{
	m_brush = brush;
}

/*!
  Vrati lavy segment ciary. Ak takyto segment neexistuje vrati \e NULL.

  \sa setLeftLineSegment
*/
SNDiagramLineSegment *SNDiagramControlPoint::leftLineSegment() const
{
	return m_leftSegment;
}

/*!
  Vrati pravy segment ciary. Ak takyto segment neexistuje vrati \e NULL.

  \sa setRightLineSegment
*/
SNDiagramLineSegment *SNDiagramControlPoint::rightLineSegment() const
{
	return m_rightSegment;
}

/*!
  Nastavenie laveho segmentu ciary. Pri presune kontrolneho bodu sa bude jedna
  strana segmentu ciary presuvat.

  \sa leftLineSegment
*/
void SNDiagramControlPoint::setLeftLineSegment(SNDiagramLineSegment *segment)
{
	m_leftSegment = segment;
	updateLinePositions();
}

/*!
  Nastavenie praveho segmentu ciary.

  \sa rightLineSegment
*/
void SNDiagramControlPoint::setRightLineSegment(SNDiagramLineSegment *segment)
{
	m_rightSegment = segment;
	updateLinePositions();
}

/*!
  Nastavenie kontajneru, ktory obsahuje kontrolne body a segmenty ciary.
*/
void SNDiagramControlPoint::setLine(SNDiagramLine *line)
{
	m_line = line;
}

/*!
  Vrati kontajner obsahujuci kontrolne body a segmenty ciary.
*/
SNDiagramLine *SNDiagramControlPoint::line() const
{
	return m_line;
}

/*!
  Vrati \e true, ak je tento bod perzistentny (tj. neodstranuje sa pri odstraneni
  ciary).
*/
bool SNDiagramControlPoint::persistent() const
{
	return m_persistent;
}

/*!
  Nastavenie toho, ci kontrolny bod ma byt perzistentny.
*/
void SNDiagramControlPoint::setPersistent(bool persistent)
{
	m_persistent = persistent;
}

/*!
  Zistenie, ci je mozne pridat do kontrolneho bodu dalsi segment ciary. Konektory
  su vzdy kontrolnymi bodmi, preto mozu mat nastavenu jedinu ciaru. Bezne
  kontrolne body maju vstupujuci a vystupujuci segment ciary.
*/
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

/*!
  Vrati typ kontrolneho bodu.
*/
int SNDiagramControlPoint::type() const
{
	return ControlPoint;
}

/*!
  Aktualizacia laveho a praveho segmentu ciary (ak taky existuje) pri presune
  kontrolneho bodu.

  \sa setLeftLineSegment, setRightLineSegment
*/
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

/*!
  \class SNDiagramLineSegment
  \brief Segment ciary sprajajucej zariadenia.
  \ingroup widgets

  Trieda SNDiagramLineSegment vykresluje jeden segment ciary sprajajucej
  kontrolne body.
 */

/*!
  Vytvorenie segmentu ciary.

  \param[in] line Kontajner reprezentujuci celu ciaru, v ktorom je tento segment.
  \param[in] x1 X-ova pozicia prveho bodu.
  \param[in] y1 Y-ova pozicia prveho bodu.
  \param[in] x2 X-ova pozicia druheho bodu.
  \param[in] y2 Y-ova pozicia druheho bodu.
  \param[in] pen Styl ciary.
*/
SNDiagramLineSegment::SNDiagramLineSegment(SNDiagramLine *line, qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen)
	: QGraphicsLineItem(x1, y1, x2, y2)
{
	setPen(pen);
	setFlag(ItemIsSelectable);
	m_line = line;
}

/*!
  Kontajner \a line sa nastavi na \e NULL.

  \par

  \overload
*/
SNDiagramLineSegment::SNDiagramLineSegment(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen)
	: QGraphicsLineItem(x1, y1, x2, y2)
{
	setPen(pen);
	setFlag(ItemIsSelectable);
	m_line = NULL;
}

/*!
  \reimp
*/
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

/*!
  \reimp
*/
QRectF SNDiagramLineSegment::boundingRect() const
{
	return shape().controlPointRect();
}

/*!
  \reimp
*/
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

/*!
  Vrati smernik na kontajner, v ktorom sa nachadza tento segment.
*/
SNDiagramLine *SNDiagramLineSegment::parentLine() const
{
	return m_line;
}

/* ------------------------------------------------------------------ */

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
	: m_scene(scene), m_persistentPoints(0), m_mapLine(mapLine)
{
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
  kontrolny bod, ktory je konetorom).
*/
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
  Vrati smernik na ciaru v mape.
*/
SNMapLineItem *SNDiagramLine::mapLine() const
{
	return m_mapLine;
}
