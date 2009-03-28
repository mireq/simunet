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
#ifndef SNDEVICESLISTMODEL_H
#define SNDEVICESLISTMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <string>

class SNSimulate;
class QItemSelectionModel;

/**
 @author Miroslav Bendik
*/
class SNDevicesListModel : public QAbstractItemModel
{
		Q_OBJECT
	public:
		SNDevicesListModel(QObject* parent = 0);
		~SNDevicesListModel();

		// informacie pre treeView
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index ) const;
		QVariant data(const QModelIndex &index, int role) const;
		bool setData(const QModelIndex &index, const QVariant &value, int role);
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;

		// drag & drop
		Qt::DropActions supportedDropActions() const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QStringList mimeTypes() const;
		QMimeData *mimeData(const QModelIndexList &indexes) const;
		bool dropMimeData(const QMimeData *data, Qt::DropAction action,
		                  int row, int column, const QModelIndex &parent);

		// praca so stromom
		uint32_t startDevice(const std::string &filename, const QModelIndex &index = QModelIndex());
		void addDirectory(const QString &name, const QModelIndex &index = QModelIndex());
		bool removeDevice(const QModelIndex &index);
		QItemSelectionModel *selectionModel() const;
	private:
		void insertCompute(const QModelIndex &index, int &parent, int &row, QModelIndex &parentIndex);
		SNSimulate *m_simulate;
		QItemSelectionModel *m_selection;

	signals:
		void itemRemoved(int devId);

};

#endif
