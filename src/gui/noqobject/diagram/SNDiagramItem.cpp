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
#include "SNDiagramItem.h"

#include <QGraphicsScene>
#include <QDebug>

/*!
  \class SNDiagramItem
  \brief Polozka diagramu so zariadeniami.
  \ingroup widgets
 */

/*!
  Vytovrenie novej polozky na pozicii [0, 0].
*/
SNDiagramItem::SNDiagramItem()
		: QGraphicsItem(), m_pos(0, 0)
{
}

/*!
  Zrusenie polozky.
*/
SNDiagramItem::~SNDiagramItem()
{
}

/*!
  Nastavenie novej pozicie na \a pos. Metoda zabezpecuje to, aby polozka nemohla
  byt mimo sceny.
*/
void SNDiagramItem::setPos(const QPointF &pos)
{
	m_pos = pos;
	if ((scene() != NULL) && !scene()->sceneRect().contains(pos))
	{
		QRectF sr = scene()->sceneRect();
		QPointF newPos = pos;
		if (newPos.x() < sr.x())
		{
			newPos.setX(sr.x());
		}
		else if (newPos.x() > sr.x() + sr.width())
		{
			newPos.setX(sr.x() + sr.width());
		}

		if (newPos.y() < sr.y())
		{
			newPos.setY(sr.y());
		}
		else if (newPos.y() > sr.y() + sr.height())
		{
			newPos.setY(sr.y() + sr.height());
		}
		QGraphicsItem::setPos(newPos);
	}
	else
	{
		QGraphicsItem::setPos(pos);
	}
}

/*!
  \overload
*/
void SNDiagramItem::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

/*!
  Zistenie aktualnej pozicie.
*/
QPointF SNDiagramItem::pos() const
{
	return m_pos;
}


