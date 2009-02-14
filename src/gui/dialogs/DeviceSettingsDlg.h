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
#ifndef DEVICESETTINGSDLG_H
#define DEVICESETTINGSDLG_H

#include <QDialog>

class SNWebConfigWidget;
class QStatusBar;
class QPushButton;
class QTreeWidget;
class QSplitter;
class QProgressBar;

/**
 @author Miroslav Bendik <miroslav.bendik@gmail.com>
*/
class DeviceSettingsDlg : public QDialog
{
		Q_OBJECT
	public:
		DeviceSettingsDlg(QWidget* parent = 0);
		~DeviceSettingsDlg();
		QStatusBar *statusBar();
	protected:
		bool event(QEvent *event);
	private:
		void saveWindowState();
		void restoreWindowState();
	private slots:
		void currentTabItemChanged(int index);
		void switchJsErrors();
		void showJsErrors();
		void hideJsErrors();
		void setErrorsAvitable(bool avitable);
		void showWebLink(const QString &link, const QString &title, const QString &content);
		void setProgress(int progress);
		void showProgressBar();
		void hideProgressBar();
	private:
		QStatusBar *m_statusBar;
		QPushButton *m_errorButton;
		QTreeWidget *m_errorsTree;
		QSize m_initialSize;
		bool m_errorsVisible;
		bool m_loadProgressVisible;
		SNWebConfigWidget *m_webConfig;
		QProgressBar *m_loadProgress;
};

#endif
