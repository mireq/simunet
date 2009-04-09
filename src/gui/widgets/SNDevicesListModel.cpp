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
#include "core/SNAccessors.h"

#include <QMimeData>
#include <QItemSelectionModel>

#include <string>
#include <list>

#include <QDebug>

using namespace std;

/*!
  \class SNDevicesListModel
  \brief Model pomocou ktoreho sa manipuluje so zariadeniami.
  \ingroup models

  Pomocou tejto triedy sa daju modifikovat zariadenia v SNSimulate. Zariadenia
  by sa nemali modifikovat priamo aby boli zobrazene zariadenia aktualne.
*/

/*!
  Vytvorenie modelu.

  Pri vytvoreni modelu sa vytvori objekt SNSimulate.
*/
SNDevicesListModel::SNDevicesListModel(QObject* parent):
		QAbstractItemModel(parent)
{
	SNConfig config;
	m_simulate = SNSingleton::getSimulate();
	m_selection = new QItemSelectionModel(this);
}

/*!
  Zrusenie modelu.
*/
SNDevicesListModel::~SNDevicesListModel()
{
}

/*!
  \reimp

  \par Vrati pocet riadkov ktore su v adresari urcenom parametrom \a parent.
*/
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

/*!
  \reimp

  \par Vrati data na pozicii indexu. Pre ziskanie ID zariadenia musi byt rola
  Qt::UserRole.
*/
QVariant SNDevicesListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole)
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

/*!
  \reimp

  \par Nastavenie mena polozky.
*/
bool SNDevicesListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && index.internalId() < 0 && role == Qt::EditRole)
	{
		m_simulate->renameDirectory(value.toString().toUtf8().data(), index.internalId());
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

/*!
  Spustenie zariadenia v adresari urcenom indexom.
*/
uint32_t SNDevicesListModel::startDevice(const std::string &filename, const QModelIndex &index)
{
	int parent = 0;
	int row = 0;
	QModelIndex parentIndex;
	insertCompute(index, parent, row, parentIndex);

	beginInsertRows(parentIndex, row, row);
	uint32_t devId = m_simulate->startDevice(filename, parent, row);
	endInsertRows();

	return devId;
}

/*!
  Vytvorenie podadresara v adresari urceonom indexom.
*/
void SNDevicesListModel::addDirectory(const QString &name, const QModelIndex &index)
{
	int parent = 0;
	int row = 0;
	QModelIndex parentIndex;
	insertCompute(index, parent, row, parentIndex);


	beginInsertRows(parentIndex, row, row);
	m_simulate->addDirectory(name.toUtf8().data(), parent, row);
	endInsertRows();
}

/*!
  Odstranenie zariadenia z modelu. Zariadenie je urcene indexom.
*/
bool SNDevicesListModel::removeDevice(const QModelIndex &index)
{
	if (!index.isValid())
	{
		return true;
	}

	int id = index.internalId();
	int i = m_simulate->findIndexOfDevice(id);
	if (i == -1)
	{
		return true;
	}

	bool ret;
	beginRemoveRows(index.parent(), i, i);
	if (id > 0)
	{
		ret = m_simulate->stopDevice(id);
		emit itemRemoved(id);
	}
	else
	{
		const list<int> odstranene = m_simulate->removeDirectory(id);
		if (odstranene.size() > 0)
		{
			ret = false;
			list<int>::const_iterator it;
			for (it = odstranene.begin(); it != odstranene.end(); ++it)
			{
				emit itemRemoved(*it);
			}
		}
		else
		{
			ret = true;
		}
	}
	endRemoveRows();
	return ret;
}

/*!
  \reimp
*/
Qt::DropActions SNDevicesListModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

/*!
  \reimp
*/
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
			return Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable | defaultFlags;
		}
	}
	else
	{
		return Qt::ItemIsDropEnabled | defaultFlags;
	}
}

/*!
  \reimp

  \par Podporovany mime typ je application/x-simunet-device.
*/
QStringList SNDevicesListModel::mimeTypes() const
{
	QStringList types;
	types << "application/x-simunet-device";
	return types;
}

/*!
  \reimp
*/
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

/*!
  \reimp
*/
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
		riadok = rowCount(parent);
	else
		return false;
		//riadok = rowCount(QModelIndex());


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
		int devParent = m_simulate->parent(deviceId);
		int i = m_simulate->findIndexOfDevice(deviceId, devParent);
		if (i == -1)
		{
			return false;
		}


		QModelIndex parentIndex;
		if (devParent != 0)
		{
			parentIndex = createIndex(m_simulate->findIndexOfDevice(devParent), 0, devParent);
		}
		beginRemoveRows(parentIndex, i, i);
		m_simulate->removeFromSubtree(deviceId, devParent);
		endRemoveRows();

		int newParent = 0;
		if (parent.isValid())
		{
			newParent = parent.internalId();
		}
		else
		{
			newParent = 0;
		}

		if (devParent == newParent && i < riadok)
		{
			riadok--;
		}

		if (newParent == 0)
		{
			parentIndex = QModelIndex();
		}
		else
		{
			parentIndex = createIndex(m_simulate->findIndexOfDevice(newParent), 0, newParent);
		}
		beginInsertRows(parentIndex, riadok, riadok);
		m_simulate->addToSubtree(deviceId, riadok, newParent);
		endInsertRows();
		m_selection->setCurrentIndex(createIndex(riadok, 0, deviceId), QItemSelectionModel::Current);
		m_selection->select(createIndex(riadok, 0, deviceId), QItemSelectionModel::SelectCurrent);
	}
	return true;
}

/*!
  Vrati model vyberu.

  \warning Ak sa nenastavi spravny model vyberu pre pohlad nebude mozne po
  presune zariadenia / adresara ponechat vybranu polozku.
*/
QItemSelectionModel *SNDevicesListModel::selectionModel() const
{
	return m_selection;
}

/*!
  \reimp
*/
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

/*!
  \reimp
*/
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

	int row = m_simulate->findIndexOfDevice(parent, pParent);

	if (row < 0)
	{
		return QModelIndex();
	}

	return createIndex(row, 0, parent);
}

/*!
  \reimp
*/
int SNDevicesListModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}


/*!
  Tato metoda vrati smernik na zariadenie specifikovane indexom modelu.
  Ak zariadenie neexistuje vrati tato metoda NULL.
*/
SNDevice *SNDevicesListModel::device(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return NULL;
	}
	else
	{
		return m_simulate->device(index.internalId());
	}
}

void SNDevicesListModel::insertCompute(const QModelIndex &index, int &parent, int &row, QModelIndex &parentIndex)
{
	if (index.isValid())
	{
		// ak je nadradeny prvok adresar
		if (index.internalId() < 0)
		{
			parentIndex = index;
			parent = index.internalId();
			const vector<int> *deviceIds = m_simulate->devicesList(parent);
			if (deviceIds != NULL)
			{
				row = deviceIds->size();
			}
		}
		else
		{
			parentIndex = index.parent();
			if (parentIndex.isValid())
			{
				parent = parentIndex.internalId();
			}
			else
			{
				parent = 0;
			}
			row = m_simulate->findIndexOfDevice(index.internalId(), parent) + 1;
		}
	}
	else
	{
		const vector<int> *deviceIds = m_simulate->devicesList(parent);
		if (deviceIds != NULL)
		{
			row = deviceIds->size();
		}
	}
}

// ----------------------------------------------------------------

/*!
  \fn void SNDevicesListModel::itemRemoved(int devId)

  Signál je vyvolaný po odstránení položky zo zoznamu zariadení. V prípade
  záporného devId ide o adresár, kladné devId signalizuje odstránenie zariadenia.

  Po odstránení adresára sa podadresáre prehľadávajú do hĺbky.
*/

