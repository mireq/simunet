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

#include "simunet.h" 
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QMetaType>

#include "gui/dialogs/MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	// registracia (zatial neznameho) typu uint32_t
	qRegisterMetaType<uint32_t>("uint32_t");

	/*QFile cssFile(":guiStyle.css");
	if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		app.setStyleSheet(cssFile.readAll());
		cssFile.close();
	}*/
	QCoreApplication::setOrganizationName("SimuNet");
	QCoreApplication::setOrganizationDomain("simunet.eu");
	QCoreApplication::setApplicationName("SimuNet");

	QTranslator qtTranslator;
	QTranslator appTranslator;

	qtTranslator.load(QString("qt_") + QLocale::system().name());
	appTranslator.load(QString("simunet_") + QLocale::system().name());

	app.installTranslator(&qtTranslator);
	app.installTranslator(&appTranslator);

	MainWindow mainwin;
	mainwin.show();
	int returnCode = app.exec();

	return returnCode;
}
