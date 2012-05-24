/*
    detailsbutton.h
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


#ifndef DETAILSBUTTON_H
#define DETAILSBUTTON_H

#include <QPushButton>

class Controller;

class DetailsButton : public QPushButton {
	
	/*
	 * This button represents an ATC "Details" button in Airport Details Window.
	 */
	
	Q_OBJECT
	
public:
	DetailsButton(const Controller*, QWidget* parent = 0);
	
signals:
	void clicked(const Controller*);
	
public slots:
	void handleClicked();
	
private:
	const Controller * __current;
	
	
};

#endif // DETAILSBUTTON_H
