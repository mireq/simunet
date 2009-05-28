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
#ifndef SNDEVICESDIAGRAMSCENE_H
#define SNDEVICESDIAGRAMSCENE_H

#include <QGraphicsScene>
#include <QColor>
#include <QSet>
#include <QMap>

#include "core/SNGuiSettings.h"

#include "SNAbstractDevicesScene.h"

class SNDiagramConnector;
class SNDiagramLineSegment;
class SNDiagramControlPoint;
class SNDiagramDevice;
class SNDiagramLine;

/**
 @author Miroslav Bendik
*/
class SNDevicesDiagramScene : public QGraphicsScene, public SNAbstractDevicesScene
{
		Q_OBJECT
	public:
		/// Nastavenie rezimu prace so scenou.
		enum SceneMode
		{
			Norm, /*!< Standardny rezim sceny. */
			Line /*!< Rezim v ktorom sa pridavaju ciary. */
		};
		SNDevicesDiagramScene(QObject *parent = 0);
		~SNDevicesDiagramScene();
		void addDevice(SNMapDeviceItem *item);
		void removeDevice(SNMapDeviceItem *item);
		void updateDevice(SNMapDeviceItem *item);

		void addHwPort(SNMapDeviceItem *item, port_num hwPort);
		void removeHwPort(SNMapDeviceItem *item, port_num hwPort);

		void resizeSceneView(const QSize &size);
	protected:
		void drawBackground(QPainter *painter, const QRectF &rect);
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
		void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	private:
		void mousePressEventNorm(QGraphicsSceneMouseEvent *event);
		void mousePressEventLine(QGraphicsSceneMouseEvent *event);
		void mouseMoveEventNorm(QGraphicsSceneMouseEvent *event);
		void mouseMoveEventLine(QGraphicsSceneMouseEvent *event);
		void newPoint(const QPointF &point);
		void removeControlPoint(SNDiagramControlPoint *point, bool forceRemovePersistent = false);
		void mergeLine(SNDiagramControlPoint *point);
		void updateSceneGeometry();
		void processPendingEvents();
		void mouseHover(QGraphicsSceneMouseEvent *event);

	private slots:
		void colorChanged(const QColor &color, SNGuiSettings::ColorGroup group);

	private:
/*!
  Farba ciary medzi kontrolnymi bodmi.
*/
		QColor m_controlPointLineColor;

/*!
  Farba pozadia kontrolneho bodu.
*/
		QColor m_controlPointBgColor;

/*!
  Farba  pozadia sceny.
*/
		QColor m_sceneBgColor;

/*!
  Farba mriezky.
*/
		QColor m_sceneGridColor;
		static const int m_gridSize;

/*!
  Aktualny rezim prace so scenou.
*/
		SceneMode m_mode;

/*!
  Novy bod v rezime pridavania ciary. Automaticky sa presuva na poziciu kurzoru.
*/
		SNDiagramControlPoint *m_newPoint;

/*!
  Koncovy bod, na ktory uzivatel klikol v rezime pridavania ciary.
*/
		SNDiagramControlPoint *m_endControlPointClicked;

/*!
  Zoznam prepojeni kontrolnych bodov.
*/
		QSet<SNDiagramLine *> m_lines;

/*!
  Zoznam zariadeni zobrazenych v diagrame.
*/
		QMap<SNMapDeviceItem *, SNDiagramDevice *> m_devices;

/*!
  Zoznam portov. Pouziva sa pri aktualizacii informacii o zariadeni.
*/
		QMap<uint32_t, QMap<port_num, SNDiagramConnector *> >m_ports;

/*!
  Udrziava velkost pohladu na scenu.
*/
		QSize m_viewSize;

/*!
  Pomocna premenna do ktorej sa uklada nova velkost sceny pred jej zmenou.
*/
		QRectF m_newSceneRect;

/*!
  Pomocna premenna indikujuca to, ze treba aktualizovat oblast grafickej sceny.
*/
		bool m_updateSceneRect;

/*!
  Zoznam ciar nachadzajucich sa pod kurzorom.
*/
		QSet<SNDiagramLine *> m_hoverLines;

		friend class SNDiagramDevice;
		friend class SNDiagramItem;
};

#endif
