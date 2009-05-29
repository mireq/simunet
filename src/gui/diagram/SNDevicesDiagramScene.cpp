/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2009 by Miroslav Bendik                                 *
 *   miroslav.bendik@gmail.com                                             *
 *                                                                         *
 *   This program is free software; you can rem_gridSizeribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is m_gridSizeributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "SNDevicesDiagramScene.h"
#include "SNDiagramLine.h"
#include "SNDiagramDevice.h"

#include "core/SNDevice.h"
#include "core/SNAccessors.h"
#include "core/SNDynamicSettings.h"
#include "core/map/SNMap.h"
#include "core/map/SNPoint3f.h"
#include "core/map/SNMapDeviceItem.h"


#include <QPainter>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

#include <QPair>
#include <vector>

#include <QDebug>

using namespace std;

/*!
  Sirka medzier v mriezke.
*/
const int SNDevicesDiagramScene::m_gridSize = 10;


/*!
  \class SNDevicesDiagramScene
  \brief Scena zobrazujuca zariadenia.
  \ingroup widgets
 */

/*!
  Vytvorenie novej grafickej sceny.
*/
SNDevicesDiagramScene::SNDevicesDiagramScene(QObject* parent)
	:QGraphicsScene(parent), m_mode(Norm),
	m_newPoint(NULL), m_endControlPointClicked(NULL), m_updateSceneRect(false)
{
	SNGuiSettings *settings = SNSingleton::getDynSettings<SNGuiSettings>();
	QPalette palette;
	QColor hgColor = palette.color(QPalette::Highlight);
	QColor bgColor = palette.color(QPalette::Base);

	// aktualizacia farieb pri zmene nastaveni
	connect(settings, SIGNAL(colorChanged(const QColor &, SNGuiSettings::ColorGroup)), SLOT(colorChanged(const QColor &, SNGuiSettings::ColorGroup)));
	m_sceneBgColor = settings->color(SNGuiSettings::BgColor);
	m_sceneGridColor = settings->color(SNGuiSettings::GridColor);

	m_controlPointLineColor = hgColor;
	m_controlPointBgColor = QColor((bgColor.red() * 5 + hgColor.red()) / 6, (bgColor.green() * 5 + hgColor.green()) / 6, (bgColor.blue() * 5 + hgColor.blue()) / 6);
	m_controlPointBgColor.setAlpha(180);
}

/*!
  Zrusenie sceny a prvkov v nej.
*/
SNDevicesDiagramScene::~SNDevicesDiagramScene()
{
	QSet<SNDiagramLine *>::iterator it;
	for (it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		delete (*it);
	}
}

/*!
  Pridanie zariadenia do zoznamu zairadeni a aktualizacia jeho vlasntosti.

  \reimp
*/
void SNDevicesDiagramScene::addDevice(SNMapDeviceItem *item)
{
	SNAbstractDevicesScene::addDevice(item);
	SNDiagramDevice *dev = new SNDiagramDevice(item, this);
	addItem(dev);
	m_devices[item] = dev;
	updateDevice(item);

	updateSceneGeometry();
	processPendingEvents();
}

/*!
  \reimp
*/
void SNDevicesDiagramScene::removeDevice(SNMapDeviceItem *item)
{
	QMap<SNMapDeviceItem *, SNDiagramDevice *>::iterator dev;
	dev = m_devices.find(item);
	if (dev != m_devices.end())
	{
		dev.value()->setDevice(NULL);
		removeItem(dev.value());
		const QMap<port_num, SNDiagramConnector *> *con = dev.value()->connectors();
		foreach(SNDiagramConnector *connector, *con)
		{
			removeControlPoint(connector, true);
		}
		delete dev.value();
		m_devices.erase(dev);
	}
	SNAbstractDevicesScene::removeDevice(item);

	updateSceneGeometry();
	processPendingEvents();
}

/*!
  Aktualizacia portov (ak boli niektore pridane pridaju sa aj do sceny, ak)
  boli odstranene, potom sa zo sceny odstrania.

  \reimp
*/
void SNDevicesDiagramScene::updateDevice(SNMapDeviceItem *item)
{
	QMap<SNMapDeviceItem *, SNDiagramDevice *>::const_iterator dev;
	dev = m_devices.find(item);
	if (dev != m_devices.end())
	{
		SNPoint3f pos = dev.key()->pos();
		SNMapDeviceItem *mapItem = dev.key();
		SNDiagramDevice *diagramItem = dev.value();
		diagramItem->setName(QString::fromUtf8(mapItem->name().c_str()));
		diagramItem->setPos(pos.x(), pos.y());
	}

	SNDevice *device = dev.key()->device();
	const vector<port_num> portNumbers = device->portNumbers();
	vector<port_num>::const_iterator port;
	QMap<uint32_t, QMap<port_num, SNDiagramConnector *> >::const_iterator devPortsList;
	devPortsList = m_ports.find(item->deviceId());


	for (port = portNumbers.begin(); port != portNumbers.end(); ++port)
	{
		if (devPortsList == m_ports.end())
		{
			addHwPort(item, *port);
		}
		else if (!devPortsList.value().contains(*port))
		{
			addHwPort(item, *port);
		}
	}

	QSet<port_num> portSet;
	for (port = portNumbers.begin(); port != portNumbers.end(); ++port)
	{
		portSet.insert(*port);
	}

	if (devPortsList != m_ports.end())
	{
		QMap<port_num, SNDiagramConnector *>::const_iterator portIter;
		for (portIter = devPortsList.value().begin(); portIter != devPortsList.value().end(); ++portIter)
		{
			if (!portSet.contains(portIter.key()))
			{
				removeHwPort(item, portIter.key());
			}
		}
	}

	updateSceneGeometry();
	processPendingEvents();
}

/*!
  \reimp
*/
void SNDevicesDiagramScene::addHwPort(SNMapDeviceItem *item, port_num hwPort)
{
	QMap<SNMapDeviceItem *, SNDiagramDevice *>::iterator device;
	device = m_devices.find(item);
	if (device == m_devices.end())
	{
		return;
	}

	SNDiagramDevice *dev = device.value();
	SNDiagramConnector *conn = dev->addConnector(hwPort);
	addItem(conn);

	m_ports[item->deviceId()][hwPort] = conn;
	updateDevice(item);

	updateSceneGeometry();
	processPendingEvents();
}

/*!
  \reimp
*/
void SNDevicesDiagramScene::removeHwPort(SNMapDeviceItem *item, port_num hwPort)
{
	QMap<SNMapDeviceItem *, SNDiagramDevice *>::iterator device;
	device = m_devices.find(item);
	if (device == m_devices.end())
	{
		return;
	}

	SNDiagramDevice *dev = device.value();
	dev->removeConnector(hwPort);

	updateSceneGeometry();
	processPendingEvents();
}

/*!
  Aktualizacia velkosti sceny podla pohladu na scenu.
*/
void SNDevicesDiagramScene::resizeSceneView(const QSize &size)
{
	m_viewSize = size;
	updateSceneGeometry();
	processPendingEvents();
}


/*!
  Vukreslenie pozadia grafickej sceny. Scena sa vykresli farbou m_sceneBgColor,
  farba mriezky je m_sceneGridColor.
*/
void SNDevicesDiagramScene::drawBackground(QPainter *painter, const QRectF &rect)
{
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::NoBrush);

	QRectF sr = sceneRect();
	int sx = ((int)rect.x() / m_gridSize) * m_gridSize;
	int sy = ((int)rect.y() / m_gridSize) * m_gridSize;
	int endw = rect.width() + m_gridSize;
	int endh = rect.height() + m_gridSize;

	QPalette palette;
	QColor winColor = palette.color(QPalette::Window);
	painter->setBrush(winColor);
	painter->drawRect(rect);
	painter->setBrush(m_sceneBgColor);
	painter->drawRect(sr);

	painter->setPen(m_sceneGridColor);
	painter->setBrush(Qt::NoBrush);

	for (int i = 0; i < endw; i += m_gridSize)
	{
		if (i + sx < sr.x() || i + sx > sr.x() + sr.width())
		{
			continue;
		}
		painter->drawLine(sx + i, sr.y(), sx + i, sr.y() + sr.height());
	}

	for (int i = 0; i < endh; i += m_gridSize)
	{
		if (i + sy < sr.y() || i + sy > sr.y() + sr.height())
		{
			continue;
		}
		painter->drawLine(sr.x(), sy + i, sr.x() + sr.width(), sy + i);
	}
}

/*!
  Reakcia na stlacenie tlacidla mysi. Podla rezimu prace sa dalej stlacenie
  spracuje funkciami ako mousePressEventNorm, alebo mousePressEventLine.

  \reimp
*/
void SNDevicesDiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	switch(m_mode)
	{
		case Norm: mousePressEventNorm(event); break;
		case Line: mousePressEventLine(event); break;
	}
	QGraphicsScene::mousePressEvent(event);
}

/*!
  Reakcia na stlacenie tlacidla mysi v normalnom rezime.
*/
void SNDevicesDiagramScene::mousePressEventNorm(QGraphicsSceneMouseEvent *event)
{
	/*
	  Ak bolo stlacene stredne tlacidlo mysi v normalnom rezime nad kontrolnym
	  bodom potom sa tento kontrolny bod odstrani.
	*/
	if (event->buttons() & Qt::MidButton)
	{
		QGraphicsItem *item = itemAt(event->scenePos());
		if (item != 0)
		{
			SNDiagramControlPoint *point = dynamic_cast<SNDiagramControlPoint *>(item);
			if (point != 0 && point->line() != 0)
			{
				removeControlPoint(point);
				return;
			}
		}
	}
	/*
	  Ak bolo stlacne lave tlacitko nad nie plnym kontorlnym bodom (tj. moze
	  do tohto kontorlneho bodu vstupit este jeden segment ciary), potom
	  sa nastavi pomocna premenna m_endControlPointClicked obsahujuca bod, na
	  ktory uzivatel klikol.
	*/
	else if (event->buttons() & Qt::LeftButton)
	{
		QGraphicsItem *item = itemAt(event->scenePos());
		if (item != 0)
		{
			SNDiagramControlPoint *point = dynamic_cast<SNDiagramControlPoint *>(item);
			if (point != 0)
			{
				if (!point->isFull())
				{
					m_endControlPointClicked = point;
				}
			}
		}
	}
}

/*!
  Spracovanie stlaccenia tlacidla mysi v rezime pridavania ciary. Ak bolo
  stlacene ine tlacidlo nez lave ukonci sa rezim pridavania ciary.
*/
void SNDevicesDiagramScene::mousePressEventLine(QGraphicsSceneMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
	{
		m_mode = Norm;
		m_newPoint = NULL;
		m_endControlPointClicked = NULL;
	}
}

/*!
  Spracovanie pustenia tlacidla mysi.
*/
void SNDevicesDiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	/*
	  Ak bolo tlacidlo mysi stlacene nad kontrolnym bodom pri jeho pusteni sa
	  vytvori novy kontrolny bod.
	*/
	if (m_endControlPointClicked != NULL)
	{
		newPoint(event->scenePos());
		if (m_newPoint == NULL)
		{
			m_mode = Norm;
		}
		else
		{
			m_mode = Line;
		}
	}
	QGraphicsScene::mouseReleaseEvent(event);
	updateSceneGeometry();
	processPendingEvents();
}

/*!
  Reakcia na pohyb mysi. Tato funkcia preposiela data o pohybe mysi dalsim
  funkciam podla rezimu prace so scenou.
*/
void SNDevicesDiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	switch (m_mode)
	{
		case Norm: mouseMoveEventNorm(event); break;
		case Line: mouseMoveEventLine(event); break;
	}
	mouseHover(event);
	QGraphicsScene::mouseMoveEvent(event);
	processPendingEvents();
}

/*!
  Reakcia na pohyb mysi v normalnom rezime.
*/
void SNDevicesDiagramScene::mouseMoveEventNorm(QGraphicsSceneMouseEvent *event)
{
	QList<QPair<QPointF, SNDiagramItem *> > itemsToMove;

	QPointF diff = (event->scenePos() - event->lastScenePos());
	if (event->buttons() & Qt::LeftButton)
	{
		m_endControlPointClicked = NULL;

		QGraphicsItem *item;
		QList<QGraphicsItem *> items = selectedItems();
		// zistime ci hybeme ciarou, ak ano zlomime ju
		if (items.size() == 1)
		{
			SNDiagramLineSegment *segm = dynamic_cast<SNDiagramLineSegment *>(items.at(0));
			if (segm != NULL)
			{
				segm->parentLine()->breakLineSegment(segm, event->scenePos());
				event->accept();
				return;
			}
		}
		// presunieme oznacene polozky
		foreach (item, items)
		{
			SNDiagramItem *dItem = dynamic_cast<SNDiagramItem *>(item);
			if (dItem != NULL)
			{
				itemsToMove.append(QPair<QPointF, SNDiagramItem *>(dItem->pos(), dItem));
				if (!event->isAccepted())
				{
					event->accept();
				}
			}
		}
	}

	QList<QPair<QPointF, SNDiagramItem *> >::iterator itemToMove;
	for (itemToMove = itemsToMove.begin(); itemToMove != itemsToMove.end(); ++itemToMove)
	{
		itemToMove->second->setPos(itemToMove->first + diff);
	}
}

/*!
  Presun mysi v rezime kreslenia ciary.

  V tomto rezime sa presuva kontrolny bod spolu s kurzorom.
*/
void SNDevicesDiagramScene::mouseMoveEventLine(QGraphicsSceneMouseEvent *event)
{
	QPointF diff = (event->scenePos() - event->lastScenePos());
	if (m_newPoint != NULL && m_newPoint->line() != NULL)
	{
		m_newPoint->line()->movePoint(m_newPoint, diff);
	}
	event->accept();
}

/*!
  Vytvorenie noveho kontrolneho bodu na pozicii \a point.
*/
void SNDevicesDiagramScene::newPoint(const QPointF &point)
{
	if (m_endControlPointClicked == NULL)
	{
		return;
	}

	/*
	  Ak bolo kliknute na niektory nie plny kontrolny bod inej ciary potom sa tieto
	  ciary spoja.
	*/
	QList<QGraphicsItem *> it = items(point);
	QGraphicsItem *item;
	foreach(item, it)
	{
		SNDiagramControlPoint *point = dynamic_cast<SNDiagramControlPoint *>(item);
		if (point != NULL)
		{
			if (!point->isFull() && point->line() != m_endControlPointClicked->line())
			{
				mergeLine(point);
				return;
			}
		}
	}

	/*
	  Pri kliknuti na konektor nie je este na nom vytvorene ziadne prepojenie,
	  preto sa prepojenie vytvori pri pridani tohto kontrolneho bodu.
	*/
	if (m_endControlPointClicked->line() == NULL)
	{
		SNMapLineItem *mapLine = m_map->addLine();
		SNDiagramLine *l = new SNDiagramLine(this, mapLine);

		l->setControlPointsPen(m_controlPointLineColor);
		l->setControlPointsBrush(m_controlPointBgColor);
		l->setLinePen(m_controlPointLineColor);
		m_endControlPointClicked->setLine(l);
		QPointF pos = m_endControlPointClicked->pos();
		l->addControlPoint(pos.x(), pos.y(), 0, m_endControlPointClicked);
		m_lines.insert(l);
	}

	int pos = 0;
	if (m_endControlPointClicked->leftLineSegment() == NULL)
	{
		pos = 0;
	}
	else
	{
		pos = -1;
	}
	m_newPoint = m_endControlPointClicked->line()->addControlPoint(point.x(), point.y(), pos);
	m_endControlPointClicked = m_newPoint;

	updateSceneGeometry();
	processPendingEvents();
}

/*!
  Odstranenie kontorlneho bodu \a point. Ak je \a forceRemovePersistent odstrani
  sa aj taky kontrolny bod, ktory moze existovat bez prepojovacej ciary
  (napr. konektor).
*/
void SNDevicesDiagramScene::removeControlPoint(SNDiagramControlPoint *point, bool forceRemovePersistent)
{
	SNDiagramLine *line = point->line();
	if (line == NULL)
	{
		removeItem(point);
		delete point;
		return;
	}

	line->removeControlPoint(point, forceRemovePersistent);
	if (line->empty())
	{
		m_lines.remove(line);
		SNMapLineItem *mapLine = line->mapLine();
		m_map->removeLine(mapLine);
		m_hoverLines.remove(line);
		//delete line;
		line->deleteLater();
	}

	updateSceneGeometry();
	processPendingEvents();
}

/*!
  Spojenie ciary patriacej bodu \e m_endControlPointClicked a bodu
  \a point.
*/
void SNDevicesDiagramScene::mergeLine(SNDiagramControlPoint *point)
{
	SNDiagramLine *newLine = m_endControlPointClicked->line();
	SNDiagramLine *oldLine = point->line();

	int pos = -1;
	if (newLine->pointAt(0) == m_endControlPointClicked)
	{
		pos = 0;
	}


	if (oldLine == NULL)
	{
		newLine->removeControlPoint(m_endControlPointClicked);
		newLine->addControlPoint(point, pos);
	}
	else
	{
		QVector<SNDiagramControlPoint *> oldPoints = oldLine->controlPoints();
		if (oldPoints.size() == 0)
		{
			return;
		}

		if ((*oldPoints.begin()) == point)
		{
			QVector<SNDiagramControlPoint *>::iterator it;
			for (it = oldPoints.begin() + 1; it != oldPoints.end(); ++it)
			{
				SNDiagramControlPoint *p = *it;
				if (!p->persistent())
				{
					newLine->addControlPoint(p->x(), p->y(), pos);
				}
				else
				{
					newLine->addControlPoint(p, pos);
				}
			}
		}
		else if ((*(oldPoints.end() - 1)) == point)
		{
			QVectorIterator<SNDiagramControlPoint *> it(oldPoints);
			it.toBack();
			it.previous();
			while (it.hasPrevious())
			{
				SNDiagramControlPoint *p = it.previous();
				if (!p->persistent())
				{
					newLine->addControlPoint(p->x(), p->y(), pos);
				}
				else
				{
					newLine->addControlPoint(p, pos);
				}
			}
		}
		m_lines.remove(oldLine);
		SNMapLineItem *mapLine = oldLine->mapLine();
		m_map->removeLine(mapLine);
		m_hoverLines.remove(oldLine);
		//delete oldLine;
		oldLine->deleteLater();
	}

	m_newPoint = NULL;
	m_endControlPointClicked = NULL;
	return;
}

/*!
  Zmena velkosti sceny podla polohy poloziek a velkosti pohladu.
*/
void SNDevicesDiagramScene::updateSceneGeometry()
{
	QRectF sr = sceneRect();
	sr.setSize(m_viewSize);
	QRectF bRect = itemsBoundingRect();

	if (bRect.right() < m_viewSize.width())
	{
		bRect.setRight(m_viewSize.width());
	}
	if (bRect.bottom() < m_viewSize.height())
	{
		bRect.setBottom(m_viewSize.height());
	}

	if (sr.top() <= 0.0)
	{
		sr.setTop(bRect.top());
	}
	if (sr.left() <= 0.0)
	{
		sr.setLeft(bRect.left());
	}

	bRect = bRect.united(sr);
	if (bRect == sceneRect())
	{
		return;
	}
	if (bRect.x() > 0)
	{
		bRect.setX(0);
	}
	if (bRect.y() > 0)
	{
		bRect.setY(0);
	}

	m_newSceneRect = bRect;
	m_updateSceneRect = true;
}

/*!
  Spracovanie niektorych udalosti, ktore boli docasne odlozene.
*/
void SNDevicesDiagramScene::processPendingEvents()
{
	if (m_updateSceneRect == true)
	{
		setSceneRect(m_newSceneRect);
		m_updateSceneRect = false;
	}
}

/*!
  Aktualizacia poloziek pod kurzorom mysi.
 */
void SNDevicesDiagramScene::mouseHover(QGraphicsSceneMouseEvent *event)
{
	QList<QGraphicsItem *> itemsUnderCursor = items(event->scenePos());
	QGraphicsItem *item;
	QSet<SNDiagramLine *> oldLines = m_hoverLines;
	foreach(item, itemsUnderCursor)
	{
		SNDiagramLineSegment *l = dynamic_cast<SNDiagramLineSegment *>(item);
		if (l != NULL)
		{
			SNDiagramLine *line = l->parentLine();
			if (line == NULL)
			{
				continue;
			}

			if (!m_hoverLines.contains(line))
			{
				line->setControlPointsVisible(true);
				m_hoverLines.insert(line);
			}
			oldLines.remove(line);
		}
	}

	SNDiagramLine *l;
	foreach(l, oldLines)
	{
		l->setControlPointsVisible(false);
		m_hoverLines.remove(l);
	}
}

/*!
  Reakcia na zmenu farieb sceny.
*/
void SNDevicesDiagramScene::colorChanged(const QColor & color, SNGuiSettings::ColorGroup group)
{
	switch (group)
	{
		case SNGuiSettings::BgColor:
			m_sceneBgColor = color;
			break;
		case SNGuiSettings::GridColor:
			m_sceneGridColor = color;
			break;
	}
	sceneRectChanged(sceneRect());
	update(sceneRect());
}


