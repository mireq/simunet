/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2008 by Miroslav Bendik                                 *
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
#ifndef SNSCENE_H
#define SNSCENE_H

#include <QGraphicsScene>
#include <QtOpenGL>

class SNSceneAttribDialog;

/**
 @author Miroslav Bendik
*/
class SNScene : public QGraphicsScene
{
		Q_OBJECT
	public:
/*!
  Sposob navigacie v grafickej scene.
*/
		enum NavigationMode
		{
			Rotate, /*!< Mod otacania (lave tlacitko otacanie, stredne pohyb) */
			Move    /*!< Mod pohybu (lave tlacitko pohyb, stredne otacanie) */
		};
	public:
		SNScene(QObject* parent = 0);
		void setNavigationMode(NavigationMode mode);
		~SNScene();

	private:
		void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void drawBackground(QPainter *painter, const QRectF &rect);

		void zmenOtocenie(float rozdiel);
		void zmenPoziciu(const QPointF &rozdiel);
		void vykreslenieMriezky();

	private:
		bool m_tahanie;
		float m_rotacia;
		QPointF m_pozicia;
		SNSceneAttribDialog *m_dialog;
		NavigationMode m_navigationMode;

};

#endif
