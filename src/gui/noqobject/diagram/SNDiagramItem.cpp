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

#include "gui/diagram/SNDevicesDiagramScene.h"

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
	///@todo Optimalizovat
	if (m_pos == pos)
	{
		return;
	}
	const QPointF oldPos = m_pos;
	m_pos = pos;

	bool updatePending = false;
	SNDevicesDiagramScene *s = qobject_cast<SNDevicesDiagramScene *>(scene());
	if (s != NULL)
	{
		const QRectF br = boundingRect();
		const QRectF sr = scene()->sceneRect();
		if (pos.x() + br.x() < sr.x() || pos.y() + br.y() < sr.y())
		{
			updatePending = true;
		}
		else if (pos.x() + br.right() > sr.right() || pos.y() + br.bottom() > sr.bottom())
		{
			updatePending = true;
		}
	}
	QGraphicsItem::setPos(pos);
	if (updatePending && s != NULL)
	{
		s->updateSceneGeometry();
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

void SNDiagramItem::updateSceneGeometry()
{
	SNDevicesDiagramScene *s = qobject_cast<SNDevicesDiagramScene *>(scene());
	if (s != NULL)
	{
		s->updateSceneGeometry();
	}
}


