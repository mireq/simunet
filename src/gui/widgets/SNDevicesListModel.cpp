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

#include <string>

#include <QDebug>

using namespace std;

SNDevicesListModel::SNDevicesListModel(QObject* parent):
		QAbstractItemModel(parent)
{
	SNConfig config;
	m_simulate = new SNSimulate(config.threadsCount());
	m_selection = new QItemSelectionModel(this);
	m_simulate->addDirectory("Adresar", 0);
	m_simulate->addDirectory("Adresar2", -1);
	m_simulate->addDirectory("Adresar3", -1);
}


SNDevicesListModel::~SNDevicesListModel()
{
	delete m_simulate;
}

int SNDevicesListModel::rowCount(const QModelIndex &parent) const
{
	const vector<int> *deviceIds;
	if (parent.isValid())
	{
		deviceIds = m_simulate->devicesList(parent.internalId());
	}
	else
	{
		deviceIds = m_simulate->devicesList(0);
	}

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

	if (role == Qt::DisplayRole)
	{
		qint64 internalId = index.internalId();
		if (internalId > 0)
		{
			return QVariant(QString("Zariadenie %1").arg(index.internalId()));
		}
		else
		{
			string *dirName = m_simulate->directory(internalId);
			if (dirName == NULL)
			{
				return QVariant();
			}
			else
			{
				return QVariant(QString::fromUtf8(dirName->c_str(), dirName->size()));
			}
		}
	}
	else if (role == Qt::UserRole || role == Qt::ToolTipRole)
	{
		return QVariant(index.internalId());
	}
	else if (role == Qt::DecorationRole)
	{
		if (index.internalId() < 0)
		{
			return QVariant(SNIcon("folder"));
		}
		else
		{
			return QVariant(SNIcon("preferences-system-performance"));
		}
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
	endInsertRows();
	return devId;
}

bool SNDevicesListModel::stopDevice(int id)
{
	int i = m_simulate->findIndexOfDevice(id, 0);
	qDebug()<<id<<i;
	if (i == -1)
	{
		return true;
	}

	bool ret;
	beginRemoveRows(QModelIndex(), i, i);
	if (id > 0)
	{
		ret = m_simulate->stopDevice(id);
	}
	else
	{
		ret = m_simulate->removeDirectory(id);
	}
	endRemoveRows();
	return ret;
}

Qt::DropActions SNDevicesListModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags SNDevicesListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

	if (index.isValid())
	{
		if (index.internalId() > 0)
		{
			return Qt::ItemIsDragEnabled | defaultFlags;
		}
		else
		{
			return Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled | defaultFlags;
		}
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
			stream << (int)index.internalId();
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
		int i = m_simulate->findIndexOfDevice(deviceId, 0);
		if (i == -1)
		{
			return false;
		}

		beginRemoveRows(QModelIndex(), i, i);
		m_simulate->removeFromSubtree(deviceId, 0);
		endRemoveRows();
		if (i < riadok)
		{
			riadok--;
		}
		beginInsertRows(QModelIndex(), riadok, riadok);
		m_simulate->addToSubtree(deviceId, riadok, 0);
		endInsertRows();
		m_selection->setCurrentIndex(index(riadok, 0, QModelIndex()), QItemSelectionModel::Current);
		m_selection->select(index(riadok, 0, QModelIndex()), QItemSelectionModel::SelectCurrent);
	}
	return true;
}


QItemSelectionModel *SNDevicesListModel::selectionModel() const
{
	return m_selection;
}

QModelIndex SNDevicesListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column != 0)
	{
		return QModelIndex();
	}

	const std::vector<int> *list;
	if (parent.isValid())
	{
		list = m_simulate->devicesList(parent.internalId());
	}
	else
	{
		list = m_simulate->devicesList(0);
	}

	if (list == NULL)
	{
		return QModelIndex();
	}

	if (row >= (int)list->size())
	{
		return QModelIndex();
	}
	return createIndex(row, column, (*list)[row]);
}

QModelIndex SNDevicesListModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return QModelIndex();
	}

	int devId = index.internalId();

	int parent = m_simulate->parent(devId);
	if (parent == 0)
	{
		return QModelIndex();
	}

	int pParent = m_simulate->parent(parent);
	if (pParent < 0)
	{
		return QModelIndex();
	}

	int row = m_simulate->findIndexOfDevice(parent, pParent);
	if (row < 0)
	{
		return QModelIndex();
	}

	return createIndex(row, 0, parent);
}

int SNDevicesListModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

