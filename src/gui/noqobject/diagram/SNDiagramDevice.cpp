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

#include <QPalette>

#include "SNDevicesDiagramScene.h"

#include "core/map/SNMapDeviceItem.h"
#include "core/map/SNPoint3f.h"

#include <QDebug>

/*!
  \class SNDiagramConnector
  \brief Konektor v grafickej scene.
  \ingroup widgets

  Trieda SNDiagramConnector vykresuluje graficku reprezentaciu konektoru v scene.
*/

/*!
  Vytvorenie konektoru zariadenia \a device na pozicii \a x, \a y. Cislo konektoru
  je urcene argumentom \a port_num. Konektor sa bude vykreslovat s okrajom
  \a pen a vyplnou \a brush.
*/
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

/*!
  Zrusenie konektoru.
*/
SNDiagramConnector::~SNDiagramConnector()
{
}

/*!
  Nastavenie pozicie konektoru. Tato operacia presunie okrem konektoru aj
  cele zariadenie.

  \sa SNDevice::setPos
*/
void SNDiagramConnector::setPos(const QPointF &point)
{
	m_pos = point - m_diff;
	m_device->setPos(m_pos);
}

/*!
  Nastavenie pozicie podla pozicie zariadenia.
*/
void SNDiagramConnector::setDevicePos(const QPointF &point)
{
	m_pos = point;
	SNDiagramControlPoint::setPos(point + m_diff);
}

/*!
  \overload
*/
void SNDiagramConnector::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

/*!
  Nastavenie relativnej pozicie konektoru od zariadenia.
*/
void SNDiagramConnector::setItemDiff(const QPointF &diff)
{
	m_diff = diff;
	setDevicePos(m_pos);
}

/*!
  Tato metoda vrati smernik na zariadenie, ktoremu patri konektor.
*/
SNDiagramDevice *SNDiagramConnector::device() const
{
	return m_device;
}

/*!
  Vrati typ polozky grafickej sceny.
*/
int SNDiagramConnector::type() const
{
	return Connector;
}

/*!
  Vrati \e true, ak je mozne konektoru este pridat segment ciary.
*/
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

/*!
  Vrati cislo fyzickeho portu zariadenia.
*/
port_num SNDiagramConnector::hwPowrt() const
{
	return m_hwPort;
}


/* ------------------------------------------------------------------ */

/*!
  \class SNDiagramDevice
  \brief Zariadenie v grafickej scene.
  \ingroup widgets

  Trieda SNDiagramDevice vykresluje zariadenie v grafickej scene a stara sa
  o relativnu poziciu konektorov voci zariadeniu.
*/

/*!
  Vytvorenie noveho zariadenia v grafickej scene.
*/
SNDiagramDevice::SNDiagramDevice(SNMapDeviceItem *device, SNDevicesDiagramScene *scene)
	:SNDiagramItem(), m_scene(scene)
{
	setFlag(ItemIsSelectable);
	m_device = device;
	calcShape();
	m_boundingRect = m_shapeRect;
}


/*!
  Zrusenie zariadenia v grafickej scene.
*/
SNDiagramDevice::~SNDiagramDevice()
{
	/*SNDiagramConnector *conn;
	foreach(conn, m_connectors)
	{
		m_scene->removeControlPoint(conn, true);
	}*/
}

/*!
  \reimp
*/
QRectF SNDiagramDevice::boundingRect() const
{
	return m_boundingRect;
}

/*!
  \reimp
*/
void SNDiagramDevice::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QPalette palette;

//	QPen pen(Qt::black);
//	QBrush brush(Qt::white);
	QPen pen;
	QBrush brush;

	if (isSelected())
	{
		pen = QPen(palette.color(QPalette::HighlightedText));
		brush = QBrush(palette.color(QPalette::Highlight));
	}
	else
	{
		pen = QPen(palette.color(QPalette::WindowText));
		brush = QBrush(palette.color(QPalette::Base));
	}

	painter->setPen(pen);
	painter->setBrush(brush);

	painter->drawRect(m_shapeRect.x() + 1, m_shapeRect.y() + 1, m_shapeRect.width() - 2, m_shapeRect.height() - 2);

	QFontMetrics fm(m_font);

	painter->drawLine(QPointF(m_shapeRect.x() + 1, fm.height() + 3), QPointF(m_shapeRect.right() - 1, fm.height() + 3));

	QRect textRect(0, 0, fm.width(m_name), fm.height());
	painter->drawText(textRect, Qt::AlignVCenter, m_name);

	QMap<port_num, SNDiagramConnector *>::size_type row = 0;
	QMap<port_num, SNDiagramConnector *>::iterator connector;
	for (connector = m_connectors.begin(); connector != m_connectors.end(); ++connector)
	{
		textRect = QRect(0, fm.height() * (row + 1) + 5, fm.width(m_name), fm.height());
		painter->drawText(textRect, Qt::AlignVCenter, QString("Port %1").arg(connector.key()));
		row++;
	}
}

/*!
  \reimp
*/
QPainterPath SNDiagramDevice::shape() const
{
	QPainterPath path;
	path.addRect(m_shapeRect.x() + 1, m_shapeRect.y() + 1, m_shapeRect.width() - 2, m_shapeRect.height() - 2);
	return path;
	
}

/*!
  Pridanie konektoru s cislom \a port medzi konektory zariadenia.
*/
SNDiagramConnector *SNDiagramDevice::addConnector(port_num port)
{
	SNDiagramConnector *point = new SNDiagramConnector(this, 0, 0, port, QPen(Qt::black), QBrush(Qt::white));
	m_connectors[port] = point;

	calcShape();

	updateConnectorDiffs();
	point->setDevicePos(pos());

	m_boundingRect = m_shapeRect;
	update();
	return point;
}

/*!
  Odstranenie konektoru s cislom \a port zo zariadenia.
*/
void SNDiagramDevice::removeConnector(port_num port)
{
	QMap<port_num, SNDiagramConnector *>::iterator connector;
	connector = m_connectors.find(port);
	if (connector == m_connectors.end())
	{
		return;
	}

	SNDiagramConnector *oldConnector = connector.value();
	m_connectors.erase(connector);
	m_scene->removeControlPoint(oldConnector, true);

	calcShape();
	updateConnectorDiffs();
	update();
	m_boundingRect = m_shapeRect;
	updateConnectorDiffs();
}

/*!
  \reimp
*/
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

/*!
  \overload
*/
void SNDiagramDevice::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

/*!
  Vrati zariadenie asociovane s touto polozkou diagramu.
*/
SNMapDeviceItem *SNDiagramDevice::device() const
{
	return m_device;
}

/*!
  Nastavenie zariadenia asociovaneho s touto polozkou diagramu.
*/
void SNDiagramDevice::setDevice(SNMapDeviceItem *device)
{
	m_device = device;
}

/*!
  Aktualizacia relativnych pozicii konektorov voci zariadeniu.
*/
void SNDiagramDevice::updateConnectorDiffs()
{
	const int width = m_shapeRect.width();
	const int height = m_shapeRect.height();
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

/*!
  Nastavenie mena zariadenia.
*/
void SNDiagramDevice::setName(QString name)
{
	m_name = name;
	calcShape();
	update();
	updateConnectorDiffs();
	m_boundingRect = m_shapeRect;
}

/*!
  Zistenie mena zariadenia.
*/
QString SNDiagramDevice::name() const
{
	return m_name;
}

/*!
  Ziskanie zoznamu konektorov.
*/
const QMap< port_num, SNDiagramConnector *> *SNDiagramDevice::connectors() const
{
	return &m_connectors;
}

/*!
  Aktualizacia oblasti, ktoru zabera zariadenie.
 */
void SNDiagramDevice::calcShape()
{
	QFontMetrics fm(m_font);
	int width = fm.width(m_name);
	int height = fm.height();

	QMap<port_num, SNDiagramConnector *>::const_iterator connector;
	for (connector = m_connectors.begin(); connector != m_connectors.end(); ++connector)
	{
		const int w = fm.width(QString("Port %1").arg(connector.key()));
		if (w > width)
		{
			width = w;
		}
	}
	m_shapeRect = QRect(-4, -4, width + 8, height + m_connectors.count() * height + 13);

}

