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

#include <QMimeData>
#include <QItemSelectionModel>


using namespace std;

SNDevicesListModel::SNDevicesListModel(QObject* parent):
		QAbstractListModel(parent)
{
	SNConfig config;
	m_simulate = new SNSimulate(config.threadsCount());
	m_selection = new QItemSelectionModel(this);
}


SNDevicesListModel::~SNDevicesListModel()
{
	delete m_simulate;
}

int SNDevicesListModel::rowCount(const QModelIndex &parent) const
{
	const vector<int> * deviceIds = m_simulate->devicesList(0);
	if (deviceIds == NULL)
	{
		return 0;
	}
	else
	{
		return deviceIds->size();
	}
}

QVariant SNDevicesListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	const vector<int> * deviceIds = m_simulate->devicesList(0);
	if (deviceIds == NULL)
	{
		return QVariant();
	}

	if (index.row() >= (int)deviceIds->size())
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole)
	{
		return QVariant(QString("Zariadenie %1").arg((*deviceIds)[index.row()]));
	}
	else if (role == Qt::UserRole)
	{
		return QVariant((*deviceIds)[index.row()]);
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
	int count = 0;
	const vector<int> *deviceIds = m_simulate->devicesList(0);
	if (deviceIds != NULL)
	{
		count = deviceIds->size();
	}
	beginInsertRows(QModelIndex(), count, count);
	uint32_t devId = m_simulate->startDevice(filename);
	//m_deviceIds.append(devId);
	//insertRow(m_deviceIds.count());
	endInsertRows();
	return devId;
}

bool SNDevicesListModel::stopDevice(uint32_t id)
{
	int i = m_simulate->findIndexOfDevice(id, 0);
	if (i == -1)
	{
		return true;
	}

	beginRemoveRows(QModelIndex(), i, i);
	bool ret = m_simulate->stopDevice(id);
	endRemoveRows();
	return ret;
}

Qt::DropActions SNDevicesListModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags SNDevicesListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

	if (index.isValid())
	{
		return Qt::ItemIsDragEnabled | defaultFlags;
	}
	else
	{
		return Qt::ItemIsDropEnabled | defaultFlags;
	}
}

QStringList SNDevicesListModel::mimeTypes() const
{
	QStringList types;
	types << "application/x-simunet-device";
	return types;
}

QMimeData *SNDevicesListModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	foreach(QModelIndex index, indexes)
	{
		if (index.isValid())
		{
			stream << data(index, Qt::UserRole).toInt();
		}
	}

	mimeData->setData("application/x-simunet-device", encodedData);

	return mimeData;
}

bool SNDevicesListModel::dropMimeData(const QMimeData *data,
                                      Qt::DropAction action,
                                      int row, int column,
                                      const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("application/x-simunet-device"))
		return false;

	if (column > 0)
		return false;

	int riadok;
	if (row != -1)
		riadok = row;
	else if (parent.isValid())
		riadok = parent.row();
	else
		riadok = rowCount(QModelIndex());

	QByteArray encodedData = data->data("application/x-simunet-device");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QList<int> selectedDevices;
	while (!stream.atEnd())
	{
		int deviceId;
		stream >> deviceId;
		selectedDevices.push_front(deviceId);
	}

	foreach (int deviceId, selectedDevices)
	{
		m_simulate->move(deviceId, riadok, 0);
		int i = m_simulate->findIndexOfDevice(deviceId, 0);
		if (i == -1)
		{
			return false;
		}
		removeRows(i, 1, QModelIndex());
		if (i < riadok)
		{
			riadok--;
		}
		insertRows(riadok, 1, QModelIndex());
		m_selection->setCurrentIndex(index(riadok, 0, QModelIndex()), QItemSelectionModel::Current);
		m_selection->select(index(riadok, 0, QModelIndex()), QItemSelectionModel::SelectCurrent);
	}
	return true;
}


QItemSelectionModel *SNDevicesListModel::selectionModel() const
{
	return m_selection;
}

