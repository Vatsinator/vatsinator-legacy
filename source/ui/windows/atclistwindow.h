/*
    atclistwindow.h
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


#ifndef ATCLISTWINDOW_H
#define ATCLISTWINDOW_H

#include "ui/ui_atclistwindow.h"
#include "singleton.h"

class ATCListWindow :
		public QWidget,
		public Singleton< ATCListWindow >,
		private Ui::ATCListWindow {
	
	Q_OBJECT
	
public:
	ATCListWindow(QWidget* = 0);

protected:
	void resizeEvent(QResizeEvent*);
	
private:
	void __setColumnsWidths();
	void __setWindowPosition();
	
private slots:
	void __handleDoubleClicked(const QModelIndex&);
	
	
};

#endif // ATCLISTWINDOW_H