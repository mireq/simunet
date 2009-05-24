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

#include "SNMap.h"
#include "SNMapLineItem.h"
#include "SNMapControlPointItem.h"
#include "SNMapDeviceItem.h"

#include "core/SNExceptions.h"

#include <QDebug>

using namespace std;

/*!
  \class SNMapLineItem
  \brief Prepojenie medzi niekolkymi kontrolnymi bodmi.
  \ingroup map

  Trieda SNMapLineItem zdruzuje niekolko kontrolnych bodov do suvislej ciary
  medzi nimi.
 */

/*!
  Vytvorenie noveho prepojenia v mape \a map.
*/
SNMapLineItem::SNMapLineItem(SNMap *map)
{
	m_map = map;
}

/*!
  Zrusenie prepojenia na mape vratane kontrolnych bodov, ktore obsahuje.
*/
SNMapLineItem::~SNMapLineItem()
{
	vector<SNMapControlPointItem *>::iterator controlPoint;
	for (controlPoint = m_controlPoints.begin(); controlPoint != m_controlPoints.end(); ++controlPoint)
	{
		delete (*controlPoint);
	}
}

/*!
  Pridanie konektoru do prepojenia.

  Konektor sa prida na poziciu urcenu suradnicami \a x, \a y a \a z.
  Zariadenie, ktoremu patri konektor urcuje argument \a device. Cislo
  fyzickeho portu je urcene argumentom \a port a vklada sa na poradie \a pos.
*/
void SNMapLineItem::addConnector(float x, float y, float z, SNMapDeviceItem *device, port_num port, std::vector< SNMapControlPointItem * >::size_type pos)
{
	addControlPoint(new SNMapConnectorItem(SNPoint3f(x, y, z), device, port), pos);
}

/*!
  Pridanie kontrolneho bodu na prepojovaciu ciaru.

  Novy kontrolny bod so suradnicami \a x, \a y a \a z sa prida na poziciu
  \a pos.
*/
void SNMapLineItem::addControlPoint(float x, float y, float z, std::vector<SNMapControlPointItem *>::size_type pos)
{
	addControlPoint(new SNMapControlPointItem(SNPoint3f(x, y, z)), pos);
}

/*!
  Metoda na pridanie lubovolneho kontrolneho bodu (bez ohladu na to, ci je to
  konektor, alebo obycajny kontrolny bod) na poziciu \a pos. V pripade
  nespravnej hodnoty \a pos sa vyvola vynimka SNoutOfRangeException.

  V pripade, ze sa vlozia do prepojenia 2 konektory vytvori sa spojenie
  medzi 2 zariadeniami. Tato funkcia vola metodu SNMap::addConnection().
*/
void SNMapLineItem::addControlPoint(SNMapControlPointItem *point, std::vector< SNMapControlPointItem * >::size_type pos)
{
	if (pos > m_controlPoints.size())
	{
		throw SNoutOfRangeException();
	}

	m_controlPoints.insert(m_controlPoints.begin() + pos, point);

	if (point->type() == SNMapControlPointItem::Connector)
	{
		// pridanie spojenia
		if (m_connectors.size() == 1)
		{
			SNMapConnectorItem *conn1 = *(m_connectors.begin());
			SNMapConnectorItem *conn2 = static_cast<SNMapConnectorItem *>(point);
			SNMapConnection conn;
			conn.setConn1(conn1->device(), conn1->port());
			conn.setConn2(conn2->device(), conn2->port());
			m_map->addConnection(conn);
		}
		m_connectors.insert(static_cast<SNMapConnectorItem *>(point));
	}
}

/*!
  Odstranenie kontrolneho bodu z prepojenia zariadeni. Urcuje sa len pozicia,
  na ktorej sa nachadza kontrolny bod. V pripade nespravnej pozicie sa vyvola
  vynimka SNoutOfRangeException.

  V pripade, ze je vytvorene prepojenie medzi 2 zariadeniami a odstranime jeden
  z konektorov zavola takto funkcia metodu SNMap::removeConnection()
*/
void SNMapLineItem::removeControlPoint(std::vector<SNMapControlPointItem *>::size_type pos)
{
	if (pos >= m_controlPoints.size())
	{
		throw SNoutOfRangeException();
	}

	std::vector<SNMapControlPointItem *>::iterator pointIter = m_controlPoints.begin() + pos;
	SNMapControlPointItem *point = *pointIter;

	if (point->type() == SNMapControlPointItem::Connector)
	{
		std::set<SNMapConnectorItem *>::iterator connIter;
		connIter = m_connectors.find(static_cast<SNMapConnectorItem *>(point));
		if (connIter != m_connectors.end())
		{
			m_connectors.erase(connIter);
			// odstranenie spojenia
			if (m_connectors.size() == 1)
			{
				SNMapConnectorItem *conn1 = *(m_connectors.begin());
				SNMapConnectorItem *conn2 = static_cast<SNMapConnectorItem *>(point);
				SNMapConnection conn;
				conn.setConn1(conn1->device(), conn1->port());
				conn.setConn2(conn2->device(), conn2->port());
				m_map->removeConnection(conn);
			}
		}
	}

	m_controlPoints.erase(pointIter);
	delete point;
}

/* ------------------------------------------------------------------ */

/*!
  \class SNMapConnection
  \brief Prepojenie medzi 2 konektormi zariadeni.
  \ingroup map

  Tato trieda obsahuje informacie o koncovych bodoch prepojenia.
 */

/*!
  Vytvorenie noveho prepojenia.
*/
SNMapConnection::SNMapConnection()
	: m_dev1(NULL), m_dev2(NULL), m_port1(0), m_port2(0)
{
}

/*!
  Zrusenie prepojenia medzi zariadeniami.
*/
SNMapConnection::~SNMapConnection()
{
}

/*!
  Nastavenie prveho portu.
*/
void SNMapConnection::setPort1(port_num port1)
{
	m_port1 = port1;
}

/*!
  Nastavenie druheho portu.
*/
void SNMapConnection::setPort2(port_num port2)
{
	m_port2 = port2;
}

/*!
  Nastavenie prveho zariadenia.
*/
void SNMapConnection::setDev1(SNMapDeviceItem *dev1)
{
	m_dev1 = dev1;
}

/*!
  Nastavenie druheho zariadenia.
*/
void SNMapConnection::setDev2(SNMapDeviceItem *dev2)
{
	m_dev2 = dev2;
}

/*!
  Nastavenie prverho zariadenia a konektoru na nom. Tato metoda je ekvivalentom
  volania setPort1 a setDev1.
*/
void SNMapConnection::setConn1(SNMapDeviceItem *dev1, port_num port1)
{
	m_dev1 = dev1;
	m_port1 = port1;
}

/*!
  Nastavenie druheho koncoveho bodu. Tato metoda je ekvivalentom
  volania setPort2 a setDev2.
*/
void SNMapConnection::setConn2(SNMapDeviceItem *dev2, port_num port2)
{
	m_dev2 = dev2;
	m_port2 = port2;
}

/*!
  Vrati cislo prveho portu.
*/
port_num SNMapConnection::port1() const
{
	return m_port1;
}

/*!
  Vrati cislo druheho portu.
*/
port_num SNMapConnection::port2() const
{
	return m_port2;
}

/*!
  Vrati referenciu na prve zariadenie.
*/
SNMapDeviceItem *SNMapConnection::dev1() const
{
	return m_dev1;
}

/*!
  Vrati referenciu na druhe zariadenie.
*/
SNMapDeviceItem *SNMapConnection::dev2() const
{
	return m_dev2;
}

/*!
  Vrati \e true ak boli oba koncove body nastavene. V opacnom pripade vrati
  \e false.
*/
bool SNMapConnection::isValid() const
{
	if (m_dev1 == NULL || m_dev2 == NULL || m_port1 == 0 || m_port2 == 0)
	{
		return false;
	}
	return true;
}
