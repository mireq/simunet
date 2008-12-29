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

#include "SNSimulate.h"
#include "SNSimulateHelper.h"
#include "SNDevice.h"

using namespace std;

/*!
    \fn SNSimulate::SNSimulate()
    Vytvorenie 4 vlakien SNSimulateHelper
*/
SNSimulate::SNSimulate()
{
	/// @todo zmenit napevno nastaveny pocet vlakien
	m_threadCount = 4;
	m_nextDeviceId = 1;

	for (int i = 0; i < m_threadCount; ++i)
	{
		SNSimulateHelper *helper = new SNSimulateHelper();
		m_simulateHelpers.push_back(helper);
		helper->start();
	}
	m_nextSimulateHelper = m_simulateHelpers.begin();
}


SNSimulate::~SNSimulate()
{
	list<SNSimulateHelper *>::iterator helper;
	for (helper = m_simulateHelpers.begin(); helper != m_simulateHelpers.end(); ++helper)
	{
		(*helper)->stop();
	}
	for (helper = m_simulateHelpers.begin(); helper != m_simulateHelpers.end(); ++helper)
	{
		(*helper)->wait(1000);
		delete *helper;
	}
}


/*!
    \fn SNSimulate::stopDevice(uint32_t id)
 */
bool SNSimulate::stopDevice(uint32_t id)
{
    /// @todo implement me
}


/*!
    \fn SNSimulate::startDevice(const string &filename)
 */
uint32_t SNSimulate::startDevice(const string &filename)
{
	SNDevice *device = new SNDevice(filename, m_nextDeviceId);
	(*m_nextSimulateHelper)->addDevice(device);
	m_nextDeviceId++;
	m_nextSimulateHelper++;
	if (m_nextSimulateHelper == m_simulateHelpers.end())
	{
		m_nextSimulateHelper = m_simulateHelpers.begin();
	}
}


/*!
    \fn SNSimulate::processFrame(int id, PyObject *data)
 */
void SNSimulate::processFrame(uint32_t id, PyObject *data)
{
    /// @todo implement me
}


/*!
    \fn SNSimulate::telnetResponse(uint32_t id, const char *text, const char *cmd)
 */
void SNSimulate::telnetResponse(uint32_t id, const char *text, const char *cmd)
{
    /// @todo implement me
}
