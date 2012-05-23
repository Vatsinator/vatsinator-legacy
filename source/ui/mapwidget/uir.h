/*
    Uir.h
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


#ifndef UIR_H
#define UIR_H

#include <QString>
#include <QVector>

#include "ui/mapwidget/controller.h"
#include "ui/mapwidget/clickable.h"
#include "ui/mapwidget/fir.h"

class Uir : public Clickable {
	
public:
	
	ObjectType objectType() const { return UIR; }
	
	void addFir(Fir*);
	
	void addStaff(const Controller*);
	
	inline const QVector< Fir* > &
	getRange() const { return __range; }
	
	inline const QVector< const Controller* > &
	getStaff() const { return __staff; }
	
	inline void
	clear() { __staff.clear(); }
	
	QString icao;
	QString name;
	
private:
	QVector< Fir* >	__range;
	QVector< const Controller* >	__staff;
	
};

#endif // UIR_H
