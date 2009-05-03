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
#include "core/map/SNMap.h"
#include "core/map/SNDevTreeItem.h"
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
	m_map = SNSingleton::getMap();
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
	if (parent.isValid())
	{
		return m_map->numItems(parent.internalId());
	}
	else
	{
		return m_map->numItems(0);
	}
}

/*!
  \reimp

  \par Vrati data na pozicii indexu. Pre ziskanie ID zariadenia musi byt rola
  Qt::UserRole.
*/
QVariant SNDevicesListModel::data(const QModelIndex &index, int role) const
{
	/*if (!index.isValid())
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
	}*/
	if (index.isValid())
	{
		SNDevTreeItem *item = m_map->item(index.internalId());
		SNDevTreeDirectoryItem *dir = NULL;
		SNDevTreeDeviceItem *dev = NULL;

		switch(item->type())
		{
			case SNDevTreeItem::Directory:
				dir = static_cast<SNDevTreeDirectoryItem *>(item);
				break;
			case SNDevTreeItem::Device:
				dev = static_cast<SNDevTreeDeviceItem *>(item);
				break;
			default:
				break;
		}
		
		if (item == NULL)
		{
			return QVariant();
		}

		if (role == Qt::DisplayRole)
		{
			if (dir != NULL)
			{
				string dirName = dir->name();
				return QVariant(QString::fromUtf8(dirName.c_str(), dirName.size()));
			}
			else if (dev != NULL)
			{
				return QVariant(QString("Zariadenie %1").arg(dev->devId()));
			}
			else
			{
				return QVariant(item->internalId());
			}
		}
		else if (role == Qt::EditRole)
		{
			if (dir != NULL)
			{
				string dirName = dir->name();
				return QVariant(QString::fromUtf8(dirName.c_str(), dirName.size()));
			}
			return QVariant();
		}
		else if (role == Qt::DecorationRole)
		{
			if (item->type() == SNDevTreeItem::Device)
			{
				return QVariant(SNIcon("preferences-system-performance"));
			}
			else
			{
				return QVariant(SNIcon("folder"));
			}
		}
		else if (role == Qt::UserRole)
		{
			return QVariant(item->type());
		}
		else if (role == Qt::UserRole + 1)
		{
			if (item->type() == SNDevTreeItem::Device)
			{
				SNDevTreeDeviceItem *dev = static_cast<SNDevTreeDeviceItem *>(item);
				return QVariant(dev->devId());
			}
			else
			{
				return QVariant();
			}
		}
	}
	return QVariant();
}

/*!
  \reimp

  \par Nastavenie mena polozky.
*/
bool SNDevicesListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
	{
		return false;
	}

	if (!index.isValid())
	{
		return false;
	}

	SNDevTreeItem *item = m_map->item(index.internalId());
	if (item == NULL || item->type() != SNDevTreeItem::Directory)
	{
		return false;
	}

	SNDevTreeDirectoryItem *dir= static_cast<SNDevTreeDirectoryItem *>(item);
	dir->setName(value.toString().toUtf8().data());
	emit dataChanged(index, index);
	return true;
}

/*!
  Spustenie zariadenia v adresari urcenom indexom.
*/
uint32_t SNDevicesListModel::startDevice(const std::string &filename, const QModelIndex &index)
{
	uint32_t parent = 0;
	int row = 0;
	QModelIndex parentIndex;
	insertCompute(index, parent, row, parentIndex);
	
	beginInsertRows(parentIndex, row, row);
	uint32_t devId = m_map->startDevice(filename);
	m_map->insertDevice(devId, parent, row);
	endInsertRows();
	return 0;
}

/*!
  Vytvorenie podadresara v adresari urceonom indexom.
*/
void SNDevicesListModel::addDirectory(const QString &name, const QModelIndex &index)
{
	uint32_t parent = 0;
	int row = 0;
	QModelIndex parentIndex;
	insertCompute(index, parent, row, parentIndex);

	beginInsertRows(parentIndex, row, row);
	m_map->addDirectory(name.toUtf8().data(), parent, row);
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

	uint32_t parentId = 0;
	QModelIndex parent = index.parent();
	if (parent.isValid())
	{
		parentId = parent.internalId();
	}

	int row = m_map->itemIndex(index.internalId(), parentId);

	beginRemoveRows(parent, row, row);
	list<SNDevTreeItem *> itemsInTree = m_map->itemsInTree(index.internalId());
	list<SNDevTreeItem *>::iterator item;
	for (item = itemsInTree.begin(); item != itemsInTree.end(); ++item)
	{
		if ((*item)->type() == SNDevTreeItem::Device)
		{
			SNDevTreeDeviceItem *dev = static_cast<SNDevTreeDeviceItem *>(*item);
			emit itemRemoved(dev->devId());
		}
	}
	m_map->deleteItem(index.internalId(), parentId);
	//emit itemRemoved(index.internalId());
	endRemoveRows();

	return true;
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
		SNDevTreeItem *item = m_map->item(index.internalId());
		if (item == NULL)
		{
			return Qt::ItemIsDropEnabled | defaultFlags;
		}

		if (item->type() == SNDevTreeItem::Device)
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
	QList<uint32_t> selectedItems;
	while (!stream.atEnd())
	{
		uint32_t internalId;
		stream >> internalId;
		selectedItems.push_front(internalId);
	}

	uint32_t insParentId = 0;
	uint32_t insPParentId = 0;
	if (parent.isValid())
	{
		insParentId = parent.internalId();
		SNDevTreeItem *insParent = m_map->item(insParentId);
		if (insParent != NULL)
		{
			insPParentId = insParent->parentId();
		}
	}


	foreach (uint32_t internalId, selectedItems)
	{
		SNDevTreeItem *remItem = m_map->item(internalId);
		if (remItem == NULL)
		{
			return false;
		}

		uint32_t remParent = 0;
		if (remItem->parent() != NULL)
		{
			remParent = remItem->parent()->internalId();
		}

		int remRow = m_map->itemIndex(internalId, remParent);
		QModelIndex remIndex = createIndex(remRow, 0, remItem->internalId());
		QModelIndex remParentIndex = this->parent(remIndex);

		beginRemoveRows(remParentIndex, remRow, remRow);
		m_map->removeItem(internalId, remParent);
		endRemoveRows();

		uint32_t insParent = 0;
		if (parent.isValid())
		{
			insParent = parent.internalId();
		}

		if (insParent == remParent && remRow < riadok)
		{
			riadok--;
		}


		QModelIndex insParentIndex;
		if (insParent != 0)
		{
			insParentIndex = createIndex(m_map->itemIndex(insParentId, insPParentId), 0, insParentId);
		}
		beginInsertRows(insParentIndex, riadok, riadok);
		m_map->insertItem(internalId, insParent, riadok);
		endInsertRows();
	}

	/*foreach (int deviceId, selectedDevices)
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
	return true;*/
	
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
	uint32_t par = 0;
	if (parent.isValid())
	{
		par = parent.internalId();
	}
	SNDevTreeItem *item = m_map->itemAt(row, par);
	if (item == NULL)
	{
		return QModelIndex();
	}
	return createIndex(row, column, item->internalId());
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

	SNDevTreeItem *item = m_map->item(index.internalId());
	if (item == NULL)
	{
		return QModelIndex();
	}

	SNDevTreeItem *parent = item->parent();
	uint32_t parentId = 0;
	if (parent != NULL)
	{
		parentId = parent->internalId();
	}

	if (parentId == 0)
	{
		return QModelIndex();
	}

	uint32_t pparent = 0;
	if (parent->parent() != NULL)
	{
		pparent = parent->parent()->internalId();
	}

	int row = m_map->itemIndex(parentId, pparent);
	return createIndex(row, 0, parentId);
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
	/*if (!index.isValid())
	{
		return NULL;
	}
	else
	{
		return m_simulate->device(index.internalId());
	}*/
	if (!index.isValid())
	{
		return NULL;
	}

	SNDevTreeItem *item = m_map->item(index.internalId());
	if (item == NULL)
	{
		return NULL;
	}

	if (item->type() != SNDevTreeItem::Device)
	{
		return NULL;
	}

	SNDevTreeDeviceItem *dev = static_cast<SNDevTreeDeviceItem *>(item);

	return m_map->device(dev->devId());
}

void SNDevicesListModel::insertCompute(const QModelIndex &index, uint32_t &parent, int &row, QModelIndex &parentIndex)
{
	if (index.isValid())
	{
		SNDevTreeItem *item = m_map->item(index.internalId());
		if (item == NULL)
		{
			parent = 0;
			row = m_map->numItems(parent);
			return;
		}

		if (item->type() == SNDevTreeItem::Device)
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
			row = index.row() + 1;
		}
		// adresar
		else
		{
			parentIndex = index;
			parent = index.internalId();
			row = m_map->numItems(parent);
		}
	}
	else
	{
		parent = 0;
		row = m_map->numItems(parent);
	}
}

// ----------------------------------------------------------------

/*!
  \fn void SNDevicesListModel::itemRemoved(int devId)

  Signál je vyvolaný po odstránení položky zo zoznamu zariadení. V prípade
  záporného devId ide o adresár, kladné devId signalizuje odstránenie zariadenia.

  Po odstránení adresára sa podadresáre prehľadávajú do hĺbky.
*/

