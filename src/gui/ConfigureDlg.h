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
#ifndef CONFIGUREDLG_H
#define CONFIGUREDLG_H

#include <QDialog>
#include <QListWidget>

class SNTitleWidget;
class QStackedWidget;
class ConfigPanel;
class QDialogButtonBox;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
 @brief Konfiguracne okno.
*/
class ConfigureDlg : public QDialog
{
		Q_OBJECT
	public:
		ConfigureDlg(QWidget* parent = 0, Qt::WindowFlags f = 0);
		~ConfigureDlg();

/*!
 \brief Pridanie konfiguracneho panelu
*/
		void addPanel(ConfigPanel *panel);

	public slots:
/*!
 \brief Zobrazenie informacie v informacnej casti okna.
*/
		void showInfo(const QString &text, int msecs = 0);
		void showInfo(const QString &text, const QString &comment, int msecs = 0);

/*!
 \brief Zobrazenie varovania v informacnej casti okna.
*/
		void showWarning(const QString &text, int msecs = 0);
		void showWarning(const QString &text, const QString &comment, int msecs = 0);

/*!
 \brief Zobrazenie chyby v informacnej casti okna.
*/
		void showError(const QString &text, int msecs = 0);
		void showError(const QString &text, const QString &comment, int msecs = 0);

	protected:
		void closeEvent(QCloseEvent *e);
		void showEvent(QShowEvent *e);

	private:
		void showMsg(const QString &text, const QString &comment, int msecs);
		void connectPanel(ConfigPanel *panel);
		void disconnectPanel(ConfigPanel *panel);
		ConfigPanel *currentPanel();

		SNTitleWidget *m_titleWidget;
		QListWidget *m_panelsList;
		QStackedWidget *m_panel;
		QDialogButtonBox *m_buttons;

		QListWidgetItem *m_currentItem;
		QListWidgetItem *m_previousItem;

	private slots:
		void listSelectionChanged();
		void onOkClicked();
		void onCancelClicked();
		void onApplyClicked();

};

#endif
