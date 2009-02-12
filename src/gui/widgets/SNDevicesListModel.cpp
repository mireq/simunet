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
#include "SNDevicesListModel.h"

#include "SNIcon.h"
#include "core/SNConfig.h"
#include "core/SNSimulate.h"

using namespace std;

SNDevicesListModel::SNDevicesListModel(QObject* parent): QAbstractListModel(parent)
{
	SNConfig config;
	m_simulate = new SNSimulate(config.threadsCount());
}


SNDevicesListModel::~SNDevicesListModel()
{
	delete m_simulate;
}

int SNDevicesListModel::rowCount(const QModelIndex &parent) const
{
	return m_deviceIds.count();
}

QVariant SNDevicesListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	if (index.row() >= m_deviceIds.count())
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole)
	{
		return QVariant(QString("Zariadenie %1").arg(m_deviceIds[index.row()]));
	}
	else if (role == Qt::UserRole)
	{
		return QVariant(m_deviceIds[index.row()]);
	}
	else if (role == Qt::DecorationRole)
	{
		return QVariant(SNIcon("preferences-system-performance"));
	}
	else
	{
		return QVariant();
	}
}

uint32_t SNDevicesListModel::startDevice(const string & filename)
{
	uint32_t devId = m_simulate->startDevice(filename);
	beginInsertRows(QModelIndex(), m_deviceIds.count(), m_deviceIds.count());
	m_deviceIds.append(devId);
	insertRow(m_deviceIds.count());
	endInsertRows();
	return devId;
}

bool SNDevicesListModel::stopDevice(uint32_t id)
{
	bool ret = m_simulate->stopDevice(id);
	if (!ret)
	{
		int i = m_deviceIds.indexOf(id);
		if (i == -1)
		{
			return ret;
		}
		m_deviceIds.remove(i);
	}
	return ret;
}


