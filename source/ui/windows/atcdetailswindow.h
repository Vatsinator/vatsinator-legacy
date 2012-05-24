/*
    atcdetailswindow.h
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


#ifndef ATCDETAILSWINDOW_H
#define ATCDETAILSWINDOW_H

#include "ui/ui_atcdetailswindow.h"
#include "singleton.h"

class Client;
class Controller;

class ATCDetailsWindow :
		public QWidget,
		public Singleton< ATCDetailsWindow >,
		private Ui::ATCDetailsWindow {
	
	Q_OBJECT
	
public:
	ATCDetailsWindow(QWidget* = 0);
	
public slots:
	void showWindow(const Client*);
	void showWindow(const Controller*);
	
private:
	QString __produceFacility(const Controller*);
	void __showMe(const Controller*);
	void __setWindowPosition();
	
	QMap< int, QString> __ratings;
	
	
};

#endif // ATCDETAILSWINDOW_H
