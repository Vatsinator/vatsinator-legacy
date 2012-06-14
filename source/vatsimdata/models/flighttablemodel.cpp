/*
    flighttablemodel.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>
#include <algorithm>

#include "db/airportsdatabase.h"

#include "vatsimdata/pilot.h"

#include "flighttablemodel.h"
#include "defines.h"

FlightTableModel::FlightTableModel(QObject* _parent) :
		QAbstractTableModel(_parent) {}

void
FlightTableModel::addFlight(const Pilot* _p) {
	if (!__flights.contains(_p)) {
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		__flights.push_back(_p);
		endInsertRows();
	}
}

void
FlightTableModel::clear() {
	beginResetModel();
	__flights.clear();
	endResetModel();
}

const Pilot *
FlightTableModel::findFlightByCallsign(const QString& _callsign) const {
	for (const Pilot* p: __flights)
		if (p->callsign == _callsign)
			return p;
	return NULL;
}

int
FlightTableModel::rowCount(const QModelIndex&) const {
	return __flights.size();
}

int
FlightTableModel::columnCount(const QModelIndex&) const {
	/* 0 - callsign
	 * 1 - name
	 * 2 - from
	 * 3 - to
	 * 4 - aircraft
	 * 5 - button
	 */
	return 6;
}

QVariant
FlightTableModel::data(const QModelIndex& _index, int _role) const {
	if (!_index.isValid() || _index.row() >= rowCount() || _index.column() >= columnCount())
		return QVariant();
	
	switch (_role) {
		case Qt::TextAlignmentRole:
			return Qt::AlignCenter;
		case Qt::ToolTipRole:
			return __flights[_index.row()]->realName;
		case Qt::ForegroundRole:
			if (__flights[_index.row()]->prefiledOnly)
				return QBrush(QColor(Qt::gray));
			else
				return QVariant();
		case Qt::DisplayRole:
			const AirportRecord* ap;
			switch (_index.column()) {
				case Callsign:
					return __flights[_index.row()]->callsign;
				case Name:
					return __flights[_index.row()]->realName;
				case From:
					ap = AirportsDatabase::GetSingleton().find(__flights[_index.row()]->route.origin);
					if (ap)
						return QString(
								QString::fromUtf8(ap->icao) %
								" " %
								QString::fromUtf8(ap->city)
						       );
					else
						return __flights[_index.row()]->route.origin;
				case To:
					ap = AirportsDatabase::GetSingleton().find(__flights[_index.row()]->route.destination);
					if (ap)
						return QString(
								QString::fromUtf8(ap->icao) %
								" " %
								QString::fromUtf8(ap->city)
						       );
					else
						return __flights[_index.row()]->route.destination;
				case Aircraft:
					return __flights[_index.row()]->aircraft;
				case Button: // for non-prefiled-only this will be overriden by QPushButton widget
					return "Prefiled";
				default:
					return QVariant();
			}
		default:
			return QVariant();
	}
}

QVariant
FlightTableModel::headerData(int _section, Qt::Orientation _orientation, int _role) const {
	if (_section >= columnCount() || _orientation == Qt::Vertical || _role != Qt::DisplayRole)
		return QVariant();
	
	switch (_section) {
		case Callsign:
			return "Callsign";
		case Name:
			return "Name";
		case From:
			return "From";
		case To:
			return "To";
		case Aircraft:
			return "Aircraft";
		default:
			return "";
	}
}

void
FlightTableModel::sort(int _column, Qt::SortOrder _order) {
	beginResetModel();
	
	if (_column == Callsign) {
		auto comparator = [_order](const Pilot* _a, const Pilot* _b) -> bool {
			return _order == Qt::AscendingOrder ? _a->callsign < _b->callsign : _a->callsign > _b->callsign;
		};
		std::sort(__flights.begin(), __flights.end(), comparator);
	} else if (_column == Name) {
		auto comparator = [_order](const Pilot* _a, const Pilot* _b) -> bool {
			return _order == Qt::AscendingOrder ? _a->realName < _b->realName : _a->realName > _b->realName;
		};
		std::sort(__flights.begin(), __flights.end(), comparator);
	} else if (_column == From) {
		auto comparator = [_order](const Pilot* _a, const Pilot* _b) -> bool {
			return _order == Qt::AscendingOrder ? _a->route.origin < _b->route.origin :
					_a->route.origin > _b->route.origin;
		};
		std::sort(__flights.begin(), __flights.end(), comparator);
	} else if (_column == To) {
		auto comparator = [_order](const Pilot* _a, const Pilot* _b) -> bool {
			return _order == Qt::AscendingOrder ? _a->route.destination < _b->route.destination :
					_a->route.destination > _b->route.destination;
		};
		std::sort(__flights.begin(), __flights.end(), comparator);
	} else if (_column == Aircraft) {
		auto comparator = [_order](const Pilot* _a, const Pilot* _b) -> bool {
			return _order == Qt::AscendingOrder ? _a->aircraft < _b->aircraft : _a->aircraft > _b->aircraft;
		};
		std::sort(__flights.begin(), __flights.end(), comparator);
	}
	
	endResetModel();
}

