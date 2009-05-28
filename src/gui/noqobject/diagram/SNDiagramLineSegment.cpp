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
	if (m_pen == pen)
	{
		return;
	}

	m_pen = pen;
	update();
}

/*!
  Nastavenie stylu vyplne konektoru.
*/
void SNDiagramControlPoint::setBrush(const QBrush &brush)
{
	if (m_brush == brush)
	{
		return;
	}

	m_brush = brush;
	update();
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


/*!
  Vrati smernik na ciaru v mape.
*/
SNMapLineItem *SNDiagramLine::mapLine() const
{
	return m_mapLine;
}

