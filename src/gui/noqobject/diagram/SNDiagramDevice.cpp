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
	return shape().controlPointRect();
}

/*!
  \reimp
*/
void SNDiagramDevice::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->drawPath(shape());
	QFontMetrics fm(m_font);
	QRect textRect(0, 0, fm.width(m_name), fm.height());
	painter->drawText(textRect, Qt::AlignVCenter, m_name);
}

/*!
  \reimp
*/
QPainterPath SNDiagramDevice::shape() const
{
	QPainterPath path;
	QFontMetrics fm(m_font);
	int width = fm.width(m_name);
	int height = fm.height();
	path.addRect(-1, -1, width + 2, height + 2);
	return path;
}

/*!
  Pridanie konektoru s cislom \a port medzi konektory zariadenia.
*/
SNDiagramConnector *SNDiagramDevice::addConnector(port_num port)
{
	SNDiagramConnector *point = new SNDiagramConnector(this, 0, 0, port);
	m_connectors[port] = point;

	updateConnectorDiffs();
	point->setDevicePos(pos());

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
	else
	{
		SNDiagramConnector *oldConnector = connector.value();
		m_connectors.erase(connector);
		updateConnectorDiffs();

		/*m_scene->removeItem(oldConnector);
		delete oldConnector;*/
		m_scene->removeControlPoint(oldConnector, true);
	}
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
	const QRectF bRect = boundingRect();
	const int width = bRect.width();
	const int height = bRect.height();
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
	update();
	updateConnectorDiffs();
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

