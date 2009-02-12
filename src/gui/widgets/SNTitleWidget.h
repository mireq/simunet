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
#ifndef SNTITLEWIDGET_H
#define SNTITLEWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QString>
#include <SNIcon.h>

class QLabel;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
 @brief Widget na zobrazenie titulku / varovania v ramceku
*/
class SNTitleWidget : public QWidget
{
		Q_OBJECT
	public:
/*!
 \brief Typy sprav (rozlisene ikonou).
 */
		enum MessageType
		{
			PlainMessage, InfoMessage, WarningMessage, ErrorMessage
		};

	public:
		SNTitleWidget(QWidget *parent = 0);
		SNTitleWidget(const QString &text, QWidget *parent = 0);
		SNTitleWidget(const QString &text, const QIcon &icon, QWidget *parent = 0);
		SNTitleWidget(const QString &text, const QString &iconName, QWidget *parent = 0);
		SNTitleWidget(const QString &text, MessageType type = PlainMessage, QWidget *parent = 0);
		~SNTitleWidget();

	public slots:
/*!
 \brief Pomocou setPixmap je mozne pouzit vlastnu ikonu.
*/
		void setPixmap(const QString &name);
		void setPixmap(const QPixmap &pixmap);
		void setPixmap(const QIcon &icon);
		void setPixmap(MessageType type);

/*!
 \brief Nastavenie textu spravy.
 \param QString Text spravy
*/
		void setText(const QString &text);

/*!
 \brief Nastavenie komentaru.
 \param QString Text komentaru.
 Tato funkcia nastavuje volitelny komentar. Pre zrusenie zobrazenia komentaru
 treba parameter comment nastavit na NULL (QString())
*/
		void setComment(const QString &comment);

/*!
 \brief Nastavenie casu po ktorom sprava zmizne.
 \param int Cas v milisekundach. Ak je cas nulovy bude sprava zobrazena permanentne.
*/
		void setHideTimeout(int msecs);

	private:
		void setupGui();

		QLabel *m_text;
		QLabel *m_comment;
		QLabel *m_icon;

		QTimer *m_timer;

};

#endif
