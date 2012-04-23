/*
    FirsDatabase.h
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


#ifndef FIRSDATABASE_H
#define FIRSDATABASE_H

#include <QVector>
#include <QString>

#include "ui/mapwidget/Fir.h"
#include "Singleton.h"



class FirsDatabase : public Singleton< FirsDatabase > {
	
public:
	void init();
	
	inline
	const QVector< Fir > & getFirs() { return __firs; }
	
	Fir *	findFirByIcao(const QString&, bool = false);
	
	void	clearAll();
	
	void	prepareTooltips();
	
private:
	
	QVector< Fir >	__firs;
	
	bool __toolTipsPrepared;
	
	
};

#endif // FIRSDATABASE_H
