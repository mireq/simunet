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
 
#ifndef SNSPLASHSCREEN_H
#define SNSPLASHSCREEN_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPen>
#include <QBrush>

/**
 @author Miroslav Bendik
*/
class SNSplash:
	public QWidget
{
	Q_OBJECT
	public:
		SNSplash(const QString &skinPath = QString(), QWidget *parent = 0);
		~SNSplash();
		int progress() const;
		int steps() const;
		QString message() const;
		bool infoMode() const;
		void setInfoMode(bool infoMode = true);
		void moveToCenter();
		int closeTimeout() const;
		bool autoHide() const;
		bool displayed() const;
		QString skinPath() const;
		void setSkinPath(const QString &filename);
	public slots:
		void setProgress(int progress);
		void setSteps(int steps);
		void setMessage(const QString &message);
		void closeDelayed(int delay = -1);
		void setCloseTimeout(int timeout);
		void setAutoHide(bool autoHide);
		void setDisplayed(bool displayed = true);
	protected:
		virtual void paintEvent(QPaintEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void showEvent(QShowEvent *e);
		virtual void closeEvent(QCloseEvent *e);
		virtual void timerEvent(QTimerEvent *e);
	private:
		void drawProgress(QPainter &painter);
		void drawMessage(QPainter &painter);
		void stopTimer();
		void setCloseTimer(int delay);
		void updateAutoHideTimer();
		int loadSkin();
		void loadDefaultSkin();
		int m_steps;
		int m_progress;
		bool m_infoMode;
		int m_closeTimeout;
		bool m_autoHide;
		bool m_displayed;
		QString m_message;
		QString m_skinPath;
		QPixmap m_background;
		QPen m_progressBarBorder;
		QBrush m_progressBarBg;
		QBrush m_progressBarActiveBg;
		QRect m_progressPosition;
		QRect m_messagePosition;
		QColor m_messageColor;
		int m_timerId;
};

#endif

