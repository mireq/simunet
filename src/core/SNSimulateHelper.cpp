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
#include "SNSimulateHelper.h"

#include <Python.h>
#include <unistd.h>
#include <iostream>

SNSimulateHelper::SNSimulateHelper()
	:m_stop(false)
{
	Py_Initialize();
}


SNSimulateHelper::~SNSimulateHelper()
{
	std::cout<<"Ukoncujem beh"<<std::endl;
	Py_Finalize();
}

void SNSimulateHelper::run()
{
	while(1)
	{
		std::cout<<"Vlakno"<<std::endl;
		if (m_stop)
		{
			return;
		}
		sleep(1);
	}
}

void SNSimulateHelper::stop()
{
	m_stop = true;
}


